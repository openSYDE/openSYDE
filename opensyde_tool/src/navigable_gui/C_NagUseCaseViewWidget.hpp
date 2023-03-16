//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing use case specific widgets based on class C_NagUseCaseWidget

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

#ifndef C_NAGUSECASEVIEWWIDGET_H
#define C_NAGUSECASEVIEWWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "stwtypes.hpp"

#include "C_NagUseCaseWidget.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_NagUseCaseViewWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_NagUseCaseViewWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_NagUseCaseViewWidget(QWidget * const opc_Parent = NULL);
   ~C_NagUseCaseViewWidget() override;

   void SetUseCaseWidget(C_NagUseCaseWidget * const opc_Widget, const int32_t os32_Mode, const int32_t os32_SubMode,
                         const QString & orc_ItemName, const QString & orc_ItemSubSubName,
                         const bool oq_CombineItemAndSubSubName);
   void RemoveUseCaseWidget(void);

   void UpdateUseCaseWidget(const int32_t os32_SubMode, const QString & orc_ItemName, const QString & orc_ItemSubName,
                            const bool oq_CombineItemAndSubSubName);
   void UpdateItemName(const QString & orc_ItemName, const QString & orc_ItemSubName,
                       const bool oq_CombineItemAndSubSubName);

   void InitText(void) const;
   void InitBackground(void);

   void SetInteractionWidget(QWidget * const opc_Widget);

private:
   void m_HandleSubSections(void) const;

   //Avoid call
   C_NagUseCaseViewWidget(const C_NagUseCaseViewWidget &);
   C_NagUseCaseViewWidget & operator =(const C_NagUseCaseViewWidget &) &;

   Ui::C_NagUseCaseViewWidget * mpc_Ui;
   C_NagUseCaseWidget * mpc_Widget;
   int32_t ms32_Mode;
   int32_t ms32_Submode;
   QString mc_ItemName;
   QString mc_ItemSubSubName;
   bool mq_CombineItemAndSubSubName;
   static const QString mhc_SVG_ICON_PATH;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
