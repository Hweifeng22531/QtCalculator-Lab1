#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <cmath>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 状态初始化
    m_firstNum = 0;
    m_operator = '\0';
    m_isResultState = false;
    m_hasDot = false;
    ui->displayEdit->setText("0");
    setWindowTitle("计算器");

    // ========== 数字键信号槽绑定 ==========
    connect(ui->btn_0, &QPushButton::clicked, this, [=](){ inputNumber("0"); });
    connect(ui->btn_1, &QPushButton::clicked, this, [=](){ inputNumber("1"); });
    connect(ui->btn_2, &QPushButton::clicked, this, [=](){ inputNumber("2"); });
    connect(ui->btn_3, &QPushButton::clicked, this, [=](){ inputNumber("3"); });
    connect(ui->btn_4, &QPushButton::clicked, this, [=](){ inputNumber("4"); });
    connect(ui->btn_5, &QPushButton::clicked, this, [=](){ inputNumber("5"); });
    connect(ui->btn_6, &QPushButton::clicked, this, [=](){ inputNumber("6"); });
    connect(ui->btn_7, &QPushButton::clicked, this, [=](){ inputNumber("7"); });
    connect(ui->btn_8, &QPushButton::clicked, this, [=](){ inputNumber("8"); });
    connect(ui->btn_9, &QPushButton::clicked, this, [=](){ inputNumber("9"); });

    // ========== 基础功能键绑定 ==========
    connect(ui->btn_dot, &QPushButton::clicked, this, &MainWindow::inputDot);
    connect(ui->btn_clear, &QPushButton::clicked, this, &MainWindow::clearAll);
    connect(ui->btn_ce, &QPushButton::clicked, this, &MainWindow::clearCurrent);
    connect(ui->btn_back, &QPushButton::clicked, this, &MainWindow::backspace);
    connect(ui->btn_sign, &QPushButton::clicked, this, &MainWindow::toggleSign);
    connect(ui->btn_percent, &QPushButton::clicked, this, &MainWindow::percentCalc);

    // ========== 单目运算键绑定 ==========
    connect(ui->btn_reciprocal, &QPushButton::clicked, this, &MainWindow::reciprocal);
    connect(ui->btn_square, &QPushButton::clicked, this, &MainWindow::square);
    connect(ui->btn_sqrt, &QPushButton::clicked, this, &MainWindow::sqrtCalc);

    // ========== 双目运算符与等号绑定 ==========
    connect(ui->btn_add, &QPushButton::clicked, this, [=](){ inputOperator('+'); });
    connect(ui->btn_sub, &QPushButton::clicked, this, [=](){ inputOperator('-'); });
    connect(ui->btn_mul, &QPushButton::clicked, this, [=](){ inputOperator('*'); });
    connect(ui->btn_div, &QPushButton::clicked, this, [=](){ inputOperator('/'); });
    connect(ui->btn_equal, &QPushButton::clicked, this, &MainWindow::calculateResult);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// ==================== 1. 数字与小数点输入 ====================
void MainWindow::inputNumber(QString num)
{
    // 计算出结果后，输入新数字自动清空，开启新计算
    if(m_isResultState) {
        ui->displayEdit->clear();
        m_isResultState = false;
        m_hasDot = false;
    }

    QString current = ui->displayEdit->text();
    // 首位为0时，新数字直接替换，避免001这类无效格式
    if(current == "0" && num != ".") {
        ui->displayEdit->setText(num);
    } else {
        ui->displayEdit->insert(num);
    }
}

// 防重复小数点核心逻辑
void MainWindow::inputDot()
{
    if(m_isResultState) {
        ui->displayEdit->setText("0.");
        m_isResultState = false;
        m_hasDot = true;
        return;
    }
    // 通过标记位控制：已有小数点则直接忽略输入
    if(!m_hasDot) {
        ui->displayEdit->insert(".");
        m_hasDot = true;
    }
}

// ==================== 2. 清除与退格 ====================
void MainWindow::clearAll()
{
    m_firstNum = 0;
    m_operator = '\0';
    m_isResultState = false;
    m_hasDot = false;
    ui->displayEdit->setText("0");
}

void MainWindow::clearCurrent()
{
    if(m_isResultState) {
        clearAll();
        return;
    }
    m_hasDot = false;
    ui->displayEdit->setText("0");
}

void MainWindow::backspace()
{
    if(m_isResultState) return;

    QString text = ui->displayEdit->text();
    if(text.length() == 1 || (text.length() == 2 && text.startsWith("-"))) {
        ui->displayEdit->setText("0");
        m_hasDot = false;
    } else {
        if(text.endsWith(".")) m_hasDot = false;
        ui->displayEdit->backspace();
    }
}

void MainWindow::toggleSign()
{
    double num = ui->displayEdit->text().toDouble();
    num = -num;
    ui->displayEdit->setText(QString::number(num, 'g', 12));
}

void MainWindow::percentCalc()
{
    double num = ui->displayEdit->text().toDouble();
    ui->displayEdit->setText(QString::number(num / 100.0, 'g', 12));
    m_isResultState = true;
}

// ==================== 3. 四则运算与状态管理 ====================
void MainWindow::inputOperator(QChar op)
{
    // 连续运算：已有未完成运算时，先自动计算上一步结果
    if(m_operator != '\0' && !m_isResultState) {
        calculateResult();
    }
    // 保存第一个操作数和运算符
    m_firstNum = ui->displayEdit->text().toDouble();
    m_operator = op;
    // 重置小数点标记，准备输入第二个数
    m_hasDot = false;
    ui->displayEdit->setText("0");
}

void MainWindow::calculateResult()
{
    if(m_operator == '\0') return;

    double secondNum = ui->displayEdit->text().toDouble();
    double result = 0;

    switch(m_operator.toLatin1()) {
    case '+': result = m_firstNum + secondNum; break;
    case '-': result = m_firstNum - secondNum; break;
    case '*': result = m_firstNum * secondNum; break;
    case '/':
        // 除零异常拦截
        if(secondNum == 0) {
            ui->displayEdit->setText("错误：除数不能为0");
            m_isResultState = true;
            m_operator = '\0';
            return;
        }
        result = m_firstNum / secondNum;
        break;
    default: break;
    }

    ui->displayEdit->setText(QString::number(result, 'g', 12));
    m_isResultState = true;
    m_operator = '\0';
    m_hasDot = ui->displayEdit->text().contains(".");
}

// ==================== 4. 单目运算 ====================
void MainWindow::reciprocal()
{
    double num = ui->displayEdit->text().toDouble();
    if(num == 0) {
        ui->displayEdit->setText("错误：除数不能为0");
        m_isResultState = true;
        return;
    }
    ui->displayEdit->setText(QString::number(1.0 / num, 'g', 12));
    m_isResultState = true;
}

void MainWindow::square()
{
    double num = ui->displayEdit->text().toDouble();
    ui->displayEdit->setText(QString::number(num * num, 'g', 12));
    m_isResultState = true;
}

void MainWindow::sqrtCalc()
{
    double num = ui->displayEdit->text().toDouble();
    if(num < 0) {
        ui->displayEdit->setText("错误：负数不能开平方");
        m_isResultState = true;
        return;
    }
    ui->displayEdit->setText(QString::number(sqrt(num), 'g', 12));
    m_isResultState = true;
}

// ==================== 5. 键盘事件（逻辑完全复用） ====================
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch(event->key()) {
    // 数字键
    case Qt::Key_0: case Qt::Key_1: case Qt::Key_2:
    case Qt::Key_3: case Qt::Key_4: case Qt::Key_5:
    case Qt::Key_6: case Qt::Key_7: case Qt::Key_8:
    case Qt::Key_9:
        inputNumber(QString::number(event->key() - Qt::Key_0));
        break;
    // 小数点
    case Qt::Key_Period:
        inputDot();
        break;
    // 运算符
    case Qt::Key_Plus:  inputOperator('+'); break;
    case Qt::Key_Minus: inputOperator('-'); break;
    case Qt::Key_Asterisk: inputOperator('*'); break;
    case Qt::Key_Slash: inputOperator('/'); break;
    // 等于（回车/等号键）
    case Qt::Key_Enter: case Qt::Key_Return: case Qt::Key_Equal:
        calculateResult();
        break;
    // 退格
    case Qt::Key_Backspace:
        backspace();
        break;
    // Esc=C全清；Delete=CE清当前D
    case Qt::Key_Escape:
        clearAll();
        break;
    case Qt::Key_Delete:
        clearCurrent();
        break;
    default:
        QMainWindow::keyPressEvent(event);
    }
}