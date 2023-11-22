//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Horizontal splitter for navigation (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGESPIHORIZONTALNAVIGATION_HPP
#define C_OGESPIHORIZONTALNAVIGATION_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeSpiBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeSpiHorizontalNavigation :
   public C_OgeSpiBase
{
   Q_OBJECT

public:
   C_OgeSpiHorizontalNavigation(QWidget * const opc_Parent = NULL);

protected:
   QSplitterHandle * createHandle(void) override;
};

class C_OgeSpiHorizontalNavigationHandle :
   public QSplitterHandle
{
   Q_OBJECT

public:
   C_OgeSpiHorizontalNavigationHandle(const Qt::Orientation oe_Orientation, QSplitter * const opc_Parent);

protected:
   void paintEvent(QPaintEvent * const opc_Event) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
