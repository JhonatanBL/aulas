import torch


# ==========================================
# UTILS — IoU e Non-Maximum Suppression
# ==========================================
# Compatível com o formato de caixas usado em yolov8_predict.py:
#   [y_min, x_min, y_max, x_max]  (coordenadas em pixels da imagem original)
# ==========================================


def compute_iou(box: torch.Tensor, boxes: torch.Tensor) -> torch.Tensor:
    """
    Calcula o IoU (Intersection over Union) entre uma caixa e um conjunto de caixas.

    Parâmetros
    ----------
    box   : Tensor [4]   — caixa de referência no formato [y_min, x_min, y_max, x_max]
    boxes : Tensor [N, 4] — N caixas candidatas no mesmo formato

    Retorna
    -------
    iou : Tensor [N] com os valores de IoU em [0, 1]

    Fórmula
    -------
                  |A ∩ B|
    IoU(A, B) = ───────────
                |A ∪ B|

    onde  |A ∪ B| = |A| + |B| - |A ∩ B|
    """
    # Coordenadas da caixa de referência
    y_min_a = box[0]
    x_min_a = box[1]
    y_max_a = box[2]
    x_max_a = box[3]

    # Coordenadas de todas as caixas candidatas
    y_min_b = boxes[:, 0]
    x_min_b = boxes[:, 1]
    y_max_b = boxes[:, 2]
    x_max_b = boxes[:, 3]

    # ── Interseção ──────────────────────────────────────────────────────────────
    # O retângulo de interseção é delimitado pelo máximo dos mínimos e o mínimo
    # dos máximos em cada eixo.  Se o resultado for negativo, não há sobreposição.
    inter_y_min = torch.maximum(y_min_a, y_min_b)
    inter_x_min = torch.maximum(x_min_a, x_min_b)
    inter_y_max = torch.minimum(y_max_a, y_max_b)
    inter_x_max = torch.minimum(x_max_a, x_max_b)

    inter_h = torch.clamp(inter_y_max - inter_y_min, min=0.0)
    inter_w = torch.clamp(inter_x_max - inter_x_min, min=0.0)
    intersection = inter_h * inter_w

    # ── Áreas individuais ────────────────────────────────────────────────────────
    area_a = (y_max_a - y_min_a) * (x_max_a - x_min_a)
    area_b = (y_max_b - y_min_b) * (x_max_b - x_min_b)

    # ── União ────────────────────────────────────────────────────────────────────
    union = area_a + area_b - intersection

    # Evita divisão por zero (caixas degeneradas com área nula)
    iou = torch.where(union > 0.0, intersection / union, torch.zeros_like(union))
    return iou


def manual_nms(boxes: torch.Tensor, scores: torch.Tensor,
               iou_threshold: float = 0.4) -> torch.Tensor:
    """
    Non-Maximum Suppression (NMS) implementado manualmente em PyTorch puro.

    O algoritmo percorre as caixas ordenadas por score (decrescente) e suprime
    qualquer caixa subsequente cujo IoU com a caixa selecionada exceda o limiar.

    Parâmetros
    ----------
    boxes         : Tensor [N, 4] no formato [y_min, x_min, y_max, x_max]
    scores        : Tensor [N]    scores de confiança de cada caixa
    iou_threshold : float         IoU acima do qual uma caixa é suprimida (padrão 0.4)

    Retorna
    -------
    keep : Tensor [K] com os índices das K caixas mantidas (K ≤ N),
           ordenados do score mais alto para o mais baixo.

    Complexidade
    ------------
    O(N²) no pior caso — adequado para detecção de objetos típica onde N ≤ 8400
    após filtragem por score de confiança (na prática N ≪ 8400).

    Exemplo de uso
    --------------
    >>> keep = manual_nms(boxes, scores, iou_threshold=0.4)
    >>> boxes_finais  = boxes[keep]
    >>> scores_finais = scores[keep]
    """
    if boxes.numel() == 0:
        return torch.empty(0, dtype=torch.long, device=boxes.device)

    # Ordena os índices do score mais alto para o mais baixo
    order = torch.argsort(scores, descending=True)

    keep = []

    while order.numel() > 0:
        # 1. Seleciona a caixa com o maior score restante
        best_idx = order[0].item()
        keep.append(best_idx)

        if order.numel() == 1:
            break  # Última caixa — nada mais a comparar

        # 2. Compara a caixa selecionada com todas as restantes
        best_box      = boxes[best_idx]          # [4]
        remaining     = boxes[order[1:]]         # [M, 4]

        iou = compute_iou(best_box, remaining)   # [M]

        # 3. Mantém apenas as caixas com IoU ≤ threshold (sem sobreposição excessiva)
        suppress_mask = iou <= iou_threshold
        order = order[1:][suppress_mask]

    return torch.tensor(keep, dtype=torch.long, device=boxes.device)


# ==========================================
# UTILITÁRIOS ADICIONAIS
# ==========================================

def compute_iou_matrix(boxes_a: torch.Tensor, boxes_b: torch.Tensor) -> torch.Tensor:
    """
    Calcula a matriz de IoU completa entre dois conjuntos de caixas.

    Parâmetros
    ----------
    boxes_a : Tensor [N, 4]  formato [y_min, x_min, y_max, x_max]
    boxes_b : Tensor [M, 4]  formato [y_min, x_min, y_max, x_max]

    Retorna
    -------
    iou_matrix : Tensor [N, M]  onde iou_matrix[i, j] = IoU(boxes_a[i], boxes_b[j])

    Útil para avaliação de métricas (mAP) ou NMS por classe.
    """
    N = boxes_a.size(0)
    M = boxes_b.size(0)

    # Expande para [N, M, 4] para broadcast vetorizado
    a = boxes_a.unsqueeze(1).expand(N, M, 4)  # [N, M, 4]
    b = boxes_b.unsqueeze(0).expand(N, M, 4)  # [N, M, 4]

    inter_y_min = torch.maximum(a[..., 0], b[..., 0])
    inter_x_min = torch.maximum(a[..., 1], b[..., 1])
    inter_y_max = torch.minimum(a[..., 2], b[..., 2])
    inter_x_max = torch.minimum(a[..., 3], b[..., 3])

    inter_h = torch.clamp(inter_y_max - inter_y_min, min=0.0)
    inter_w = torch.clamp(inter_x_max - inter_x_min, min=0.0)
    intersection = inter_h * inter_w

    area_a = ((a[..., 2] - a[..., 0]) * (a[..., 3] - a[..., 1]))
    area_b = ((b[..., 2] - b[..., 0]) * (b[..., 3] - b[..., 1]))
    union = area_a + area_b - intersection

    iou_matrix = torch.where(union > 0.0, intersection / union, torch.zeros_like(union))
    return iou_matrix


def class_aware_nms(boxes: torch.Tensor, scores: torch.Tensor,
                    classes: torch.Tensor, iou_threshold: float = 0.4) -> torch.Tensor:
    """
    NMS por classe: caixas de classes diferentes nunca se suprimem mutuamente.

    Parâmetros
    ----------
    boxes         : Tensor [N, 4]  formato [y_min, x_min, y_max, x_max]
    scores        : Tensor [N]
    classes       : Tensor [N]     índice da classe predita para cada caixa
    iou_threshold : float

    Retorna
    -------
    keep : Tensor [K]  índices globais das caixas mantidas
    """
    unique_classes = classes.unique()
    keep_indices   = []

    for cls in unique_classes:
        cls_mask    = (classes == cls).nonzero(as_tuple=True)[0]
        cls_boxes   = boxes[cls_mask]
        cls_scores  = scores[cls_mask]
        cls_keep    = manual_nms(cls_boxes, cls_scores, iou_threshold)
        keep_indices.append(cls_mask[cls_keep])

    if len(keep_indices) == 0:
        return torch.empty(0, dtype=torch.long, device=boxes.device)

    keep_all = torch.cat(keep_indices)

    # Re-ordena pelo score para manter consistência com manual_nms
    reorder  = torch.argsort(scores[keep_all], descending=True)
    return keep_all[reorder]
