numeroDeLeitores = 0
numeroDeEscritores = 0
numeroEsperadoDeLeituras = 0
numeroEsperadoDeEscritas = 0
numeroRealizadoDeLeituras = 0
numeroRealizadoDeEscritas = 0
qualEscritorEstaEscrevendo = -1
ultimaThreadAEscrever = -1
quantidadeDeLeitoresLendo = 0
escritaPermitida = True
leituraPermitida = True

numeroDeFalhas = 0

def criaLeitor():
	global numeroDeLeitores
	numeroDeLeitores += 1

def criaEscritor():
	global numeroDeEscritores
	numeroDeEscritores += 1

def registraNumeroEsperadoDeLeituras(numero):
	global numeroEsperadoDeLeituras, numeroDeLeitores
	numeroEsperadoDeLeituras = numero * numeroDeLeitores

def registraNumeroEsperadoDeEscritas(numero):
	global numeroEsperadoDeEscritas, numeroDeLeitores
	numeroEsperadoDeEscritas = numero * numeroDeLeitores

def pedeAcessoParaEscrever(id):
    global numeroDeFalhas, leituraPermitida
    if not leituraPermitida or quantidadeDeLeitoresLendo > 0:
        numeroDeFalhas += 1
        print("Falha em pedir acesso para escrita pela thread {}".format(id))
    else:
        leituraPermitida = False


def pedeAcessoParaLer(id):
	global numeroDeFalhas, escritaPermitida
	if not escritaPermitida or qualEscritorEstaEscrevendo > -1:
		numeroDeFalhas += 1
		print("Falha em pedir acesso para leitura pela thread {}".format(id))
	else:
		escritaPermitida = False

def le(id):
	global numeroDeFalhas, leituraPermitida, quantidadeDeLeitoresLendo, numeroRealizadoDeLeituras, ultimaThreadAEscrever
	if not leituraPermitida or ultimaThreadAEscrever != id:
		numeroDeFalhas += 1
		print("Falha em ler, sem acesso. Thread {}".format(id))
	else:
		quantidadeDeLeitoresLendo += 1
		numeroRealizadoDeLeituras += 1

def escreve(id):
    global numeroDeFalhas, escritaPermitida, qualEscritorEstaEscrevendo, numeroRealizadoDeEscritas, ultimaThreadAEscrever
    if not escritaPermitida:
        numeroDeFalhas += 1
        print("Falha em escrever, sem acesso. Thread {}".format(id))
    else:
        qualEscritorEstaEscrevendo = id
        ultimaThreadAEscrever = id
        numeroRealizadoDeEscritas += 1

def paraLeitura(id):
    global numeroDeFalhas, leituraPermitida, quantidadeDeLeitoresLendo
    if not leituraPermitida or quantidadeDeLeitoresLendo < 1:
        numeroDeFalhas += 1
        print("Falha em parar as leitoras, elas nao estavam lendo. Thread {}".format(id))
    else:
        quantidadeDeLeitoresLendo -= 1

def paraEscrita(id):
    global numeroDeFalhas, escritaPermitida, qualEscritorEstaEscrevendo
    if not escritaPermitida or qualEscritorEstaEscrevendo == -1 or qualEscritorEstaEscrevendo != id:
        numeroDeFalhas += 1
        print("Falha em parar a escrita, ela nao estava escrevendo. Thread {}".format(id))
    else:
        qualEscritorEstaEscrevendo = -1

def liberaAcessoDaLeitora(id):
	global numeroDeFalhas, escritaPermitida
	if escritaPermitida or quantidadeDeLeitoresLendo > 0:
		numeroDeFalhas += 1
		print("Falha em liberar acesso da leitora, ela nao tinha acesso. Thread {}".format(id))
	else:
		escritaPermitida = True

def liberaAcessoDaEscritora(id):
    global numeroDeFalhas, leituraPermitida
    if leituraPermitida or qualEscritorEstaEscrevendo > -1:
        numeroDeFalhas += 1
        print("Falha em liberar acesso da escritora, ela nao tinha acesso. Thread {}".format(id))
    else:
        leituraPermitida = True

execfile("log.txt")

if numeroDeFalhas == 0 and numeroEsperadoDeEscritas == numeroRealizadoDeEscritas and numeroEsperadoDeLeituras == numeroRealizadoDeLeituras:
	print("Parabens, o programa executou como o esperado!")
else:
    print("Ops, algo deu errado. Houveram {} falhas".format(numeroDeFalhas))
