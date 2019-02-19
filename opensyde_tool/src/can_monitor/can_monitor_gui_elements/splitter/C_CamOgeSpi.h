//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Splitter with two-lines-handle (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     20.11.2018  STW/G.Landsgesell
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_CAMOGESPI_H
#define C_CAMOGESPI_H

/* -- Includes ------------------------------------------------------------- */
#include "C_OgeSpiBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

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
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual QSplitterHandle * createHandle(void) override;
   //lint -restore
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
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void mouseDoubleClickEvent(QMouseEvent * const opc_Event) override;
   virtual void paintEvent(QPaintEvent * const opc_Event) override;
   //lint -restore
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
