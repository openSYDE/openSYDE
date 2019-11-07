////----------------------------------------------------------------------------------------------------------------------
///*!
//   \file
//   \brief       Short description

//   Detailed description (optional). The module shall be described detailed if it is not described completely
//   by short description.

//   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
//*/
////----------------------------------------------------------------------------------------------------------------------

///* -- Includes ------------------------------------------------------------------------------------------------------ */
//#include "precomp_headers.h"

//#include "stwtypes.h"
//#include "C_ColorShower.h"
//#include "C_GiSyColorSelectWidget.h"

///* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
//using namespace stw_types;
//using namespace stw_opensyde_gui;

///* -- Module Global Constants --------------------------------------------------------------------------------------- */

///* -- Types --------------------------------------------------------------------------------------------------------- */

///* -- Global Variables ---------------------------------------------------------------------------------------------- */

///* -- Module Global Variables --------------------------------------------------------------------------------------- */

///* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

///* -- Implementation ------------------------------------------------------------------------------------------------ */

////----------------------------------------------------------------------------------------------------------------------
///*! \brief  Short function description

//   Detailed function description (optional). The function shall be described detailed if it is not described completely
//   by short function description and parameter description.


//   \param[in]       ou8_Aa     Detailed input parameter description
//   \param[out]      opu32_Bb   Detailed output parameter description
//   \param[in,out]   opu16_Cc   Detailed input/output parameter description

//   \return
//   Type of return values, e.g. STW error codes

//   \retval   Return value 1   Detailed description of 1st return value
//   \retval   Return value 2   Detailed description of 2nd return value
//*/
////----------------------------------------------------------------------------------------------------------------------
//C_ColorShower::C_ColorShower(C_GiSyColorSelectWidget *parent)
//    : QWidget(parent)
//{
//    colorDialog = parent;

//    curCol = qRgb(255, 255, 255);
//    curQColor = Qt::white;

//    gl = new QGridLayout(this);
//    gl->setMargin(gl->spacing());
//    lab = new QColorShowLabel(this);

//#ifndef Q_OS_WINCE
//#ifdef QT_SMALL_COLORDIALOG
//    lab->setMinimumHeight(60);
//#endif
//    lab->setMinimumWidth(60);
//#else
//    lab->setMinimumWidth(20);
//#endif

//// For QVGA screens only the comboboxes and color label are visible.
//// For nHD screens only color and luminence pickers and color label are visible.
//#if !defined(QT_SMALL_COLORDIALOG)
//    gl->addWidget(lab, 0, 0, -1, 1);
//#else
//    gl->addWidget(lab, 0, 0, 1, -1);
//#endif
//    connect(lab, SIGNAL(colorDropped(QRgb)), this, SIGNAL(newCol(QRgb)));
//    connect(lab, SIGNAL(colorDropped(QRgb)), this, SLOT(setRgb(QRgb)));

//    hEd = new QColSpinBox(this);
//    hEd->setRange(0, 359);
//    lblHue = new QLabel(this);
//#ifndef QT_NO_SHORTCUT
//    lblHue->setBuddy(hEd);
//#endif
//    lblHue->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
//#if !defined(QT_SMALL_COLORDIALOG)
//    gl->addWidget(lblHue, 0, 1);
//    gl->addWidget(hEd, 0, 2);
//#else
//    gl->addWidget(lblHue, 1, 0);
//    gl->addWidget(hEd, 2, 0);
//#endif

//    sEd = new QColSpinBox(this);
//    lblSat = new QLabel(this);
//#ifndef QT_NO_SHORTCUT
//    lblSat->setBuddy(sEd);
//#endif
//    lblSat->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
//#if !defined(QT_SMALL_COLORDIALOG)
//    gl->addWidget(lblSat, 1, 1);
//    gl->addWidget(sEd, 1, 2);
//#else
//    gl->addWidget(lblSat, 1, 1);
//    gl->addWidget(sEd, 2, 1);
//#endif

//    vEd = new QColSpinBox(this);
//    lblVal = new QLabel(this);
//#ifndef QT_NO_SHORTCUT
//    lblVal->setBuddy(vEd);
//#endif
//    lblVal->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
//#if !defined(QT_SMALL_COLORDIALOG)
//    gl->addWidget(lblVal, 2, 1);
//    gl->addWidget(vEd, 2, 2);
//#else
//    gl->addWidget(lblVal, 1, 2);
//    gl->addWidget(vEd, 2, 2);
//#endif

//    rEd = new QColSpinBox(this);
//    lblRed = new QLabel(this);
//#ifndef QT_NO_SHORTCUT
//    lblRed->setBuddy(rEd);
//#endif
//    lblRed->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
//#if !defined(QT_SMALL_COLORDIALOG)
//    gl->addWidget(lblRed, 0, 3);
//    gl->addWidget(rEd, 0, 4);
//#else
//    gl->addWidget(lblRed, 3, 0);
//    gl->addWidget(rEd, 4, 0);
//#endif

//    gEd = new QColSpinBox(this);
//    lblGreen = new QLabel(this);
//#ifndef QT_NO_SHORTCUT
//    lblGreen->setBuddy(gEd);
//#endif
//    lblGreen->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
//#if !defined(QT_SMALL_COLORDIALOG)
//    gl->addWidget(lblGreen, 1, 3);
//    gl->addWidget(gEd, 1, 4);
//#else
//    gl->addWidget(lblGreen, 3, 1);
//    gl->addWidget(gEd, 4, 1);
//#endif

//    bEd = new QColSpinBox(this);
//    lblBlue = new QLabel(this);
//#ifndef QT_NO_SHORTCUT
//    lblBlue->setBuddy(bEd);
//#endif
//    lblBlue->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
//#if !defined(QT_SMALL_COLORDIALOG)
//    gl->addWidget(lblBlue, 2, 3);
//    gl->addWidget(bEd, 2, 4);
//#else
//    gl->addWidget(lblBlue, 3, 2);
//    gl->addWidget(bEd, 4, 2);
//#endif

//    alphaEd = new QColSpinBox(this);
//    alphaLab = new QLabel(this);
//#ifndef QT_NO_SHORTCUT
//    alphaLab->setBuddy(alphaEd);
//#endif
//    alphaLab->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
//#if !defined(QT_SMALL_COLORDIALOG)
//    gl->addWidget(alphaLab, 3, 1, 1, 3);
//    gl->addWidget(alphaEd, 3, 4);
//#else
//    gl->addWidget(alphaLab, 1, 3, 3, 1);
//    gl->addWidget(alphaEd, 4, 3);
//#endif
//    alphaEd->hide();
//    alphaLab->hide();
//    lblHtml = new QLabel(this);
//    htEd = new QLineEdit(this);
//#ifndef QT_NO_SHORTCUT
//    lblHtml->setBuddy(htEd);
//#endif

//#if !defined(QT_NO_REGULAREXPRESSION)
//    QRegularExpression regExp(QStringLiteral("#([A-Fa-f0-9]{6}|[A-Fa-f0-9]{3})"));
//    QRegularExpressionValidator *validator = new QRegularExpressionValidator(regExp, this);
//    htEd->setValidator(validator);
//#elif !defined(QT_NO_REGEXP)
//    QRegExp regExp(QStringLiteral("#([A-Fa-f0-9]{6}|[A-Fa-f0-9]{3})"));
//    QRegExpValidator *validator = new QRegExpValidator(regExp, this);
//    htEd->setValidator(validator);
//#else
//    htEd->setReadOnly(true);
//#endif
//    htEd->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);

//    lblHtml->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
//#if defined(QT_SMALL_COLORDIALOG)
//    gl->addWidget(lblHtml, 5, 0);
//    gl->addWidget(htEd, 5, 1, 1, /*colspan=*/ 2);
//#else
//    gl->addWidget(lblHtml, 5, 1);
//    gl->addWidget(htEd, 5, 2, 1, /*colspan=*/ 3);
//#endif

//    connect(hEd, SIGNAL(valueChanged()), this, SLOT(hsvEd()));
//    connect(sEd, SIGNAL(valueChanged()), this, SLOT(hsvEd()));
//    connect(vEd, SIGNAL(valueChanged()), this, SLOT(hsvEd()));

//    connect(rEd, SIGNAL(valueChanged()), this, SLOT(rgbEd()));
//    connect(gEd, SIGNAL(valueChanged()), this, SLOT(rgbEd()));
//    connect(bEd, SIGNAL(valueChanged()), this, SLOT(rgbEd()));
//    connect(alphaEd, SIGNAL(valueChanged()), this, SLOT(rgbEd()));
//    connect(htEd, SIGNAL(textEdited(QString)), this, SLOT(htmlEd()));

//    retranslateStrings();

//} // unnamed namespace

////----------------------------------------------------------------------------------------------------------------------
///*! \brief   Default destructor
//*/
////----------------------------------------------------------------------------------------------------------------------
//C_ColorShower::~C_ColorShower()
//{
//   delete this;
//}

//void C_ColorShower::showAlpha(bool b)
//{
//    alphaLab->setVisible(b);
//    alphaEd->setVisible(b);
//}

//inline bool C_ColorShower::isAlphaVisible() const
//{
//    return alphaLab->isVisible();
//}

//inline QRgb C_GiSyColorSelectWidget::currentColor() const { return cs->currentColor(); }
//inline int C_GiSyColorSelectWidget::currentAlpha() const { return cs->currentAlpha(); }
//inline void C_GiSyColorSelectWidget::setCurrentAlpha(int a) { cs->setCurrentAlpha(a); }
//inline void C_GiSyColorSelectWidget::showAlpha(bool b) { cs->showAlpha(b); }
//inline bool C_GiSyColorSelectWidget::isAlphaVisible() const { return cs->isAlphaVisible(); }

//QColor C_ColorShower::currentQColor() const
//{
//    if (nativeDialogInUse)
//        return platformColorDialogHelper()->currentColor();
//    return cs->currentQColor();
//}

//void C_ColorShower::showCurrentColor()
//{
//    lab->setColor(currentColor());
//    lab->repaint();
//}

//void C_ColorShower::rgbEd()
//{
//    rgbOriginal = true;
//    curCol = qRgba(rEd->value(), gEd->value(), bEd->value(), currentAlpha());

//    rgb2hsv(currentColor(), hue, sat, val);

//    hEd->setValue(hue);
//    sEd->setValue(sat);
//    vEd->setValue(val);

//    htEd->setText(QColor(curCol).name());

//    showCurrentColor();
//    emit newCol(currentColor());
//    updateQColor();
//}

//void C_ColorShower::hsvEd()
//{
//    rgbOriginal = false;
//    hue = hEd->value();
//    sat = sEd->value();
//    val = vEd->value();

//    QColor c;
//    c.setHsv(hue, sat, val);
//    curCol = c.rgb();

//    rEd->setValue(qRed(currentColor()));
//    gEd->setValue(qGreen(currentColor()));
//    bEd->setValue(qBlue(currentColor()));

//    htEd->setText(c.name());

//    showCurrentColor();
//    emit newCol(currentColor());
//    updateQColor();
//}

//void C_ColorShower::htmlEd()
//{
//    QColor c;
//    QString t = htEd->text();
//    c.setNamedColor(t);
//    if (!c.isValid())
//        return;
//    curCol = qRgba(c.red(), c.green(), c.blue(), currentAlpha());
//    rgb2hsv(curCol, hue, sat, val);

//    hEd->setValue(hue);
//    sEd->setValue(sat);
//    vEd->setValue(val);

//    rEd->setValue(qRed(currentColor()));
//    gEd->setValue(qGreen(currentColor()));
//    bEd->setValue(qBlue(currentColor()));

//    showCurrentColor();
//    emit newCol(currentColor());
//    updateQColor();
//}

//void C_ColorShower::setRgb(QRgb rgb)
//{
//    rgbOriginal = true;
//    curCol = rgb;

//    rgb2hsv(currentColor(), hue, sat, val);

//    hEd->setValue(hue);
//    sEd->setValue(sat);
//    vEd->setValue(val);

//    rEd->setValue(qRed(currentColor()));
//    gEd->setValue(qGreen(currentColor()));
//    bEd->setValue(qBlue(currentColor()));

//    htEd->setText(QColor(rgb).name());

//    showCurrentColor();
//    updateQColor();
//}

//void C_ColorShower::setHsv(int h, int s, int v)
//{
//    if (h < -1 || (uint)s > 255 || (uint)v > 255)
//        return;

//    rgbOriginal = false;
//    hue = h; val = v; sat = s;
//    QColor c;
//    c.setHsv(hue, sat, val);
//    curCol = c.rgb();

//    hEd->setValue(hue);
//    sEd->setValue(sat);
//    vEd->setValue(val);

//    rEd->setValue(qRed(currentColor()));
//    gEd->setValue(qGreen(currentColor()));
//    bEd->setValue(qBlue(currentColor()));

//    htEd->setText(c.name());

//    showCurrentColor();
//    updateQColor();
//}

//void C_ColorShower::retranslateStrings()
//{
//    lblHue->setText(C_GiSyColorSelectWidget::tr("Hu&e:"));
//    lblSat->setText(C_GiSyColorSelectWidget::tr("&Sat:"));
//    lblVal->setText(C_GiSyColorSelectWidget::tr("&Val:"));
//    lblRed->setText(C_GiSyColorSelectWidget::tr("&Red:"));
//    lblGreen->setText(C_GiSyColorSelectWidget::tr("&Green:"));
//    lblBlue->setText(C_GiSyColorSelectWidget::tr("Bl&ue:"));
//    alphaLab->setText(C_GiSyColorSelectWidget::tr("A&lpha channel:"));
//    lblHtml->setText(C_GiSyColorSelectWidget::tr("&HTML:"));
//}

//void C_ColorShower::updateQColor()
//{
//    QColor oldQColor(curQColor);
//    curQColor.setRgba(qRgba(qRed(curCol), qGreen(curCol), qBlue(curCol), currentAlpha()));
//    if (curQColor != oldQColor)
//        emit currentColorChanged(curQColor);
//}
