//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Vertical splitter (header)

   Vertical splitter (note: main module description should be in .cpp file)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     01.02.2017  STW/S.Singer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGESPIVERTICAL_H
#define C_OGESPIVERTICAL_H

/* -- Includes ------------------------------------------------------------- */

#include <QColor>
#include "stwtypes.h"
#include "C_OgeSpiBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeSpiVertical :
   public C_OgeSpiBase
{
   Q_OBJECT

public:
   C_OgeSpiVertical(QWidget * const opc_Parent = NULL);

   void SetColor(const QColor & orc_Color) const;
   void SetMargins(const stw_types::sint32 os32_MarginTop, const stw_types::sint32 os32_MarginBottom) const;

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual QSplitterHandle * createHandle(void) override;
   //lint -restore
};

class C_OgeSpiVerticalHandle :
   public QSplitterHandle
{
public:
   C_OgeSpiVerticalHandle(const Qt::Orientation oe_Orientation, QSplitter * const opc_Parent);

   void SetColor(const QColor & orc_Color);
   void SetMargins(const stw_types::sint32 os32_MarginTop, const stw_types::sint32 os32_MarginBottom);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void paintEvent(QPaintEvent * const opc_Event) override;
   //lint -restore

private:
   QColor mc_Color;
   stw_types::sint32 ms32_MarginTop;
   stw_types::sint32 ms32_MarginBottom;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
