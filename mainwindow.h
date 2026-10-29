#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    // 重写键盘事件，实现键盘输入
    void keyPressEvent(QKeyEvent *event) override;

private:
    Ui::MainWindow *ui;

    // ========== 状态管理变量 ==========
    double m_firstNum;       // 第一个操作数
    QChar m_operator;        // 待执行的双目运算符
    bool m_isResultState;    // 当前是否为计算结果状态
    bool m_hasDot;           // 当前输入是否已有小数点

    // ========== 基础输入处理 ==========
    void inputNumber(QString num);   // 数字输入
    void inputDot();                 // 小数点输入
    void clearAll();                 // C键：全局清空
    void clearCurrent();             // CE键：清除当前输入
    void backspace();                // 退格
    void toggleSign();               // 正负号切换
    void percentCalc();              // 百分号运算

    // ========== 运算处理 ==========
    void inputOperator(QChar op);    // 双目运算符输入
    void calculateResult();          // 等号计算
    void reciprocal();               // 倒数 1/x
    void square();                   // 平方 x²
    void sqrtCalc();                 // 开平方 √x
};

#endif // MAINWINDOW_H