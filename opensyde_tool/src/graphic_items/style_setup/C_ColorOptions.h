////----------------------------------------------------------------------------------------------------------------------
///*!
//   \file
//   \brief       Short description
//   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
//*/
////----------------------------------------------------------------------------------------------------------------------
//#ifndef C_COLOROPTIONS_H
//#define C_COLOROPTIONS_H

///* -- Includes ------------------------------------------------------------------------------------------------------ */
//#include <QtCore/QObject>
//#include <QtCore/QSharedDataPointer>
//#include <QtGui/QRgb>

///* -- Namespace ----------------------------------------------------------------------------------------------------- */
//namespace stw_opensyde_gui
//{

///* -- Global Constants ---------------------------------------------------------------------------------------------- */

///* -- Types --------------------------------------------------------------------------------------------------------- */

//class Q_GUI_EXPORT C_ColorOptions
//{
//    Q_GADGET

//public:
//    enum ColorOption {
//        ShowAlphaChannel    = 0x00000001,
//        DontUseNativeDialog = 0x00000004
//    };

//    Q_DECLARE_FLAGS(ColorOptions, ColorOption)
//    Q_FLAG(ColorOptions)

//    C_ColorOptions();
//    C_ColorOptions(const C_ColorOptions &rhs);
//    C_ColorOptions &operator=(const C_ColorOptions &rhs);
//    ~C_ColorOptions();

//    void swap(C_ColorOptions &other) { qSwap(d, other.d); }

//    QString windowTitle() const;
//    void setWindowTitle(const QString &);

//    void setOption(ColorOption option, bool on = true);
//    bool testOption(ColorOption option) const;
//    void setOptions(ColorOptions options);
//    ColorOptions options() const;

//    static int customColorCount();
//    static QRgb customColor(int index);
//    static QRgb *customColors();
//    static void setCustomColor(int index, QRgb color);

//    static QRgb *standardColors();
//    static QRgb standardColor(int index);
//    static void setStandardColor(int index, QRgb color);

//private:
//    QSharedDataPointer<C_ColorOptions> d;
//};

///* -- Extern Global Variables --------------------------------------------------------------------------------------- */
//} //end of namespace


//#endif
