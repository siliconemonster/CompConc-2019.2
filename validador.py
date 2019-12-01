"""Validador de registro de execução"""
import sys

def main():
    leitores = 0
    escritores = 0
    leitores_correntes = []
    escritor_corrente = -1
    leitores_tent = []
    escritores_tent = []
    recurso = -1

    def leitor_entrou(tid, num_de_leitores):
        nonlocal leitores, leitores_correntes, leitores_tent, escritores
        leitores = leitores + 1

        if leitores != num_de_leitores:
            print('Número incompatível de leitores.')
            sys.exit(1)

        if escritores > 0:
            print('Leitor entrou enquanto escritor escrevia')
            sys.exit(1)

        leitores_tent.remove(tid)
        leitores_correntes.append(tid)

    def leitor_tentando(tid):
        nonlocal leitores_tent
        leitores_tent.append(tid)

    def leitor_saiu(tid, rec):
        nonlocal leitores, leitores_correntes
        leitores = leitores - 1

        if recurso != rec:
            print('Leitor não leu um valor correto.')
            sys.exit(1)

        leitores_correntes.remove(tid)

    def escritor_tentando(tid):
        nonlocal escritores_tent
        escritores_tent.append(tid)

    def escritor_entrou(tid):
        nonlocal escritor_corrente, escritores_tent, escritores

        if escritor_corrente != -1 or len(leitores_correntes) > 0:
            print("Escritor entrou com leitores ou escritor já ativo.")
            sys.exit(1)

        escritores = escritores + 1
        escritores_tent.remove(tid)
        escritor_corrente = tid

    def escritor_saiu(tid, rec):
        nonlocal escritor_corrente, escritores, recurso
        if escritor_corrente != tid:
            print("Escritor que saiu não é o escritor ativo.")
            sys.exit(1)

        recurso = rec
        escritor_corrente = -1
        escritores = escritores - 1


    with open(sys.argv[1]) as log:
        for linha in log:
            exec(linha)

if __name__ == '__main__':
    main()
