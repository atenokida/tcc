# Sumário

- [Sumário](#sumário)
- [Como executar](#como-executar)
  - [Pré-requisitos](#pré-requisitos)
  - [Compilar](#compilar)
  - [Executar](#executar)
- [Organização dos diretórios](#organização-dos-diretórios)

---

# Como executar

Esta seção descreve como compilar e executar os experimentos do projeto.

## Pré-requisitos

Antes de compilar, certifique-se de ter o `vcpkg` instalado.

Após instalar o `vcpkg`, utilize-o para instalar as seguintes dependências:

```
vcpkg install xtensor boost
```

## Compilar

```
cd /src/build
cmake -DCMAKE_TOOLCHAIN_FILE=~/vcpkg/scripts/buildsystems/vcpkg.cmake .. && make
```

## Executar

```
./cardinality_estimator [INPUT_FILE] [sp|tow|cms]
```

---

# Organização dos diretórios

  * `/src`: código-fonte e gerador de dados;
  * `/in`: arquivos de configuração dos experimentos;
  * `/data`: datasets;
  * `/notebooks`: Jupyter Notebooks para plotar os gráficos;
  * `/results`: diretório de saída dos resultados (tempo, estimativas, etc.).