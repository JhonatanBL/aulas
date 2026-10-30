
function ler_dados_simplex()
    print("Insira a quantidade de restricoes e variaveis (m n): ")
    m, n = parse.(Int, split(readline()))

    if m <= 0 || n < m
        throw(ArgumentError("Deve valer 0 < m <= n."))
    end

    println("Insira os $n coeficientes do vetor de custos:")
    c = parse.(Float64, split(readline()))

    println("Insira os $m valores do vetor de recursos:")
    b = parse.(Float64, split(readline()))

    A = Matrix{Float64}(undef, m, n)

    println("Insira os coeficientes da matriz A ($m x $n), linha por linha:")
    for i in 1:m
        A[i, :] = parse.(Float64, split(readline()))
    end

    println("Insira os $m indices das variaveis basicas:")
    B = parse.(Int, split(readline()))

    if length(c) != n || length(b) != m
        throw(ArgumentError("Dimensoes incorretas dos vetores c ou b."))
    end

    if size(A) != (m, n)
        throw(ArgumentError("Dimensoes incorretas da matriz A."))
    end

    if length(B) != m || length(unique(B)) != m ||
       any(j -> j < 1 || j > n, B)
        throw(ArgumentError("Base invalida: quantidade, indices ou repeticoes."))
    end

    return m, n, c, b, A, B
end

function simplex(m, n, c, b, A, B)
    tol = 1e-9

	max_iter = 10000
	iter = 0
	
	while inter < max_iter
        N = setdiff(1:n, B)
        AB = A[:, B]


        # Solucao basica
        xB = AB \ b

        if any(xB .< -tol)
            error("A base inicial nao e factivel.")
        end

        # Multiplicadores simplex
        lambda = AB' \ c[B]

        # Custos reduzidos das variaveis nao basicas

        custos = c[N] - A[:, N]' * lambda

        # Teste de otimalidade
        k = argmin(custos)

        if custos[k] >= -tol
            return 1, B
        end

        # Variavel que entra na base
        entra = N[k]

        # Direcao simplex
        d = AB \ A[:, entra]

        # Teste de ilimitacao
        indices = findall(v -> v > tol, d)

        if isempty(indices)
            return -1, B
        end

        # Teste da razao minima
        razoes = xB[indices] ./ d[indices]
        pos = argmin(razoes)
        sai_pos = indices[pos]

        # Atualiza a base na posicao da variavel que sai
        B[sai_pos] = entra
		iter = 	iter + 1

    end
end


m, n, c, b, A, B = ler_dados_simplex()

status, B = simplex(m, n, c, b, A, B)

if status == 1
    x = zeros(Float64, n)
    x[B] = A[:, B] \ b
    println("Solucao otima encontrada.")
    println("Vetor de variaveis: ", x)
    println("Variaveis basicas: ", B)
    println("Valor otimo: ", c' * x)
elseif status == -1
    println("Problema ilimitado.")
end
