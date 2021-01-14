//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Standard splitter (header)

   Standard splitter (note: main module description should be in .cpp file)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGESPIVERTICAL_H
#define C_OGESPIVERTICAL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QColor>
#include "stwtypes.h"
#include "C_OgeSpiBase.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeSpiStandard :
   public C_OgeSpiBase
{
   Q_OBJECT

public:
   C_OgeSpiStandard(QWidget * const opc_Parent = NULL);

   void SetColor(const QColor & orc_Color) const;
   void SetMargins(const stw_types::sint32 os32_MarginTop, const stw_types::sint32 os32_MarginBottom) const;

protected:
   virtual QSplitterHandle * createHandle(void) override;
};

class C_OgeSpiStandardHandle :
   public QSplitterHandle
{
public:
   C_OgeSpiStandardHandle(const Qt::Orientation oe_Orientation, QSplitter * const opc_Parent);

   void SetColor(const QColor & orc_Color);
   void SetMargins(const stw_types::sint32 os32_MarginTop, const stw_types::sint32 os32_MarginBottom);

protected:
   virtual void paintEvent(QPaintEvent * const opc_Event) override;

private:
   QColor mc_Color;
   stw_types::sint32 ms32_MarginTop;    // left margin if handle is horizontal
   stw_types::sint32 ms32_MarginBottom; // right margin if handle is horizontal
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
