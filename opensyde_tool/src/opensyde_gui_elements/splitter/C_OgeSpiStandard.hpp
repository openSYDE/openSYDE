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
#include "stwtypes.hpp"
#include "C_OgeSpiBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
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
   void SetMargins(const int32_t os32_MarginTop, const int32_t os32_MarginBottom) const;

protected:
   QSplitterHandle * createHandle(void) override;
};

class C_OgeSpiStandardHandle :
   public QSplitterHandle
{
public:
   C_OgeSpiStandardHandle(const Qt::Orientation oe_Orientation, QSplitter * const opc_Parent);

   void SetColor(const QColor & orc_Color);
   void SetMargins(const int32_t os32_MarginTop, const int32_t os32_MarginBottom);

protected:
   void paintEvent(QPaintEvent * const opc_Event) override;

private:
   QColor mc_Color;
   int32_t ms32_MarginTop;    // left margin if handle is horizontal
   int32_t ms32_MarginBottom; // right margin if handle is horizontal
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
