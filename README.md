# PGCCHIB - Processamento Gráfico

Este repositório contém as atividades realizadas na disciplina de **Processamento Gráfico - UNSINOS**.

## 📂 Estrutura do Repositório

```plaintext
📂 PGCCHIB/
├── 📂 src/                 # Código-fonte das atividades
│   ├── AtividadeVivencial  # Código-fonte das atividades vivenciais
│       ├── AtividadeM2.cpp # Código-fonte da atividade vivencial do módulo 2
├── 📄 CMakeLists.txt       # Configuração do CMake para compilar os projetos
├── 📄 README.md            # Este arquivo, com a documentação do repositório
├── 📄 GettingStarted.md    # Tutorial detalhado sobre como compilar usando o CMake
```

Siga as instruções detalhadas em [GettingStarted.md](GettingStarted.md) para configurar e compilar o projeto.

## ⚠️ **IMPORTANTE: Baixar a GLAD Manualmente**
Para que o projeto funcione corretamente, é necessário **baixar a GLAD manualmente** utilizando o **GLAD Generator**.

### 🔗 **Acesse o web service do GLAD**:
👉 [GLAD Generator](https://glad.dav1d.de/)

### ⚙️ **Configuração necessária:**
- **API:** OpenGL  
- **Version:** 3.3+ (ou superior compatível com sua máquina)  
- **Profile:** Core  
- **Language:** C/C++  

### 📥 **Baixe e extraia os arquivos:**
Após a geração, extraia os arquivos baixados e coloque-os nos diretórios correspondentes:
- Copie **`glad.h`** para `include/glad/`
- Copie **`khrplatform.h`** para `include/glad/KHR/`
- Copie **`glad.c`** para `common/`

🚨 **Sem esses arquivos, a compilação falhará!** É necessário colocar esses arquivos nos diretórios corretos, conforme a orientação acima.