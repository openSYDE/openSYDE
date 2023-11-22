//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Splitter with two-lines-handle (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMOGESPI_HPP
#define C_CAMOGESPI_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeSpiBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamOgeSpi :
   public C_OgeSpiBase
{
   Q_OBJECT

public:
   explicit C_CamOgeSpi(QWidget * const opc_Parent = NULL);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigDoubleClick(void);

protected:
   QSplitterHandle * createHandle(void) override;
};

class C_CamOgeSpiHandle :
   public QSplitterHandle
{
   Q_OBJECT

public:
   C_CamOgeSpiHandle(const Qt::Orientation oe_Orientation, QSplitter * const opc_Parent);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigDoubleClick(void);

protected:
   void mouseDoubleClickEvent(QMouseEvent * const opc_Event) override;
   void paintEvent(QPaintEvent * const opc_Event) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
