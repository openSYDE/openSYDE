//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Horizontal splitter for navigation (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     07.08.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGESPIHORIZONTALNAVIGATION_H
#define C_OGESPIHORIZONTALNAVIGATION_H

/* -- Includes ------------------------------------------------------------- */
#include "C_OgeSpiBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeSpiHorizontalNavigation :
   public C_OgeSpiBase
{
   Q_OBJECT

public:
   C_OgeSpiHorizontalNavigation(QWidget * const opc_Parent = NULL);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual QSplitterHandle * createHandle(void) override;
   //lint -restore
};

class C_OgeSpiHorizontalNavigationHandle :
   public QSplitterHandle
{
   Q_OBJECT

public:
   C_OgeSpiHorizontalNavigationHandle(const Qt::Orientation oe_Orientation, QSplitter * const opc_Parent);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void paintEvent(QPaintEvent * const opc_Event) override;
   //lint -restore
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
