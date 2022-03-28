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
#include "stwtypes.h"

#include "C_NagUseCaseWidget.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_NagUseCaseViewWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_NagUseCaseViewWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_NagUseCaseViewWidget(QWidget * const opc_Parent = NULL);
   virtual ~C_NagUseCaseViewWidget();

   void SetUseCaseWidget(C_NagUseCaseWidget * const opc_Widget, const stw_types::sint32 os32_Mode,
                         const stw_types::sint32 os32_SubMode, const QString & orc_ItemName,
                         const QString & orc_ItemSubSubName, const bool oq_CombineItemAndSubSubName);
   void RemoveUseCaseWidget(void);

   void UpdateUseCaseWidget(const stw_types::sint32 os32_SubMode, const QString & orc_ItemName,
                            const QString & orc_ItemSubName, const bool oq_CombineItemAndSubSubName);
   void UpdateItemName(const QString & orc_ItemName, const QString & orc_ItemSubName,
                       const bool oq_CombineItemAndSubSubName);

   void InitText(void) const;
   void InitBackground(void);

   void SetInteractionWidget(QWidget * const opc_Widget);

private:
   void m_HandleSubSections(void) const;

   //Avoid call
   C_NagUseCaseViewWidget(const C_NagUseCaseViewWidget &);
   C_NagUseCaseViewWidget & operator =(const C_NagUseCaseViewWidget &);

   Ui::C_NagUseCaseViewWidget * mpc_Ui;
   C_NagUseCaseWidget * mpc_Widget;
   stw_types::sint32 ms32_Mode;
   stw_types::sint32 ms32_Submode;
   QString mc_ItemName;
   QString mc_ItemSubSubName;
   bool mq_CombineItemAndSubSubName;
   static const QString mhc_SVG_ICON_PATH;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
