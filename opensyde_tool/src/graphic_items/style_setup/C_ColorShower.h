////----------------------------------------------------------------------------------------------------------------------
///*!
//   \file
//   \brief       Short description
//   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
//*/
////----------------------------------------------------------------------------------------------------------------------
//#ifndef C_COLORSHOWER_H
//#define C_COLORSHOWER_H

///* -- Includes ------------------------------------------------------------------------------------------------------ */
//#include "qlabel.h"
//#include "qlayout.h"
//#include "qlineedit.h"
//#include "C_GiSyColorSelectWidget.h"
//#include "C_ColorShowLabel.h"


///* -- Namespace ----------------------------------------------------------------------------------------------------- */
//namespace stw_opensyde_gui
//{

///* -- Global Constants ---------------------------------------------------------------------------------------------- */

///* -- Types --------------------------------------------------------------------------------------------------------- */
//class C_ColorShowLabel;

//class C_ColorShower : public QWidget
//{
//    Q_OBJECT
//public:
//    C_ColorShower(C_GiSyColorSelectWidget *parent);

//    //things that don't emit signals
//    void setHsv(int h, int s, int v);

//    int currentAlpha() const
//    { return (colorDialog->options() & C_GiSyColorSelectWidget::ShowAlphaChannel) ? alphaEd->value() : 255; }
//    void setCurrentAlpha(int a) { alphaEd->setValue(a); rgbEd(); }
//    void showAlpha(bool b);
//    bool isAlphaVisible() const;

//    QRgb currentColor() const { return curCol; }
//    QColor currentQColor() const { return curQColor; }
//    void retranslateStrings();
//    void updateQColor();

//public Q_SLOTS:
//    void setRgb(QRgb rgb);

//Q_SIGNALS:
//    void newCol(QRgb rgb);
//    void currentColorChanged(const QColor &color);

//private Q_SLOTS:
//    void rgbEd();
//    void hsvEd();
//    void htmlEd();

//private:
//    void showCurrentColor();
//    int hue, sat, val;
//    QRgb curCol;
//    QColor curQColor;
//    QLabel *lblHue;
//    QLabel *lblSat;
//    QLabel *lblVal;
//    QLabel *lblRed;
//    QLabel *lblGreen;
//    QLabel *lblBlue;
//    QLabel *lblHtml;
//    QColSpinBox *hEd;
//    QColSpinBox *sEd;
//    QColSpinBox *vEd;
//    QColSpinBox *rEd;
//    QColSpinBox *gEd;
//    QColSpinBox *bEd;
//    QColSpinBox *alphaEd;
//    QLabel *alphaLab;
//    QLineEdit *htEd;
//    C_ColorShowLabel *lab;
//    bool rgbOriginal;
//    C_GiSyColorSelectWidget *colorDialog;
//    QGridLayout *gl;

//    friend class QT_PREPEND_NAMESPACE(C_GiSyColorSelectWidget);
////    friend class QT_PREPEND_NAMESPACE(QColorDialogPrivate);
//};

///* -- Extern Global Variables --------------------------------------------------------------------------------------- */

//} //end of namespace


//#endif
