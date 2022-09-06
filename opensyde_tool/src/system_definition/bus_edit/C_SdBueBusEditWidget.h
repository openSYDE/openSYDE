//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Bus edit widget (note: main module description should be in .cpp file)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDBUEBUSEDITWIDGET_H
#define C_SDBUEBUSEDITWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QWidget>

#include "stwtypes.h"

#include "C_OSCSystemBus.h"
#include "C_SdBueComIfDescriptionWidget.h"
#include "C_SdBueBusEditPropertiesWidget.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SdBueBusEditWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueBusEditWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdBueBusEditWidget(const stw_types::uint32 ou32_BusIndex, const stw_types::sintn osn_TabIndex,
                                 QWidget * const opc_Parent = NULL);
   virtual ~C_SdBueBusEditWidget();

   void InitStaticNames(void) const;
   bool WasChanged(void) const;
   void Save(void) const;
   void SetFlag(const stw_types::uint32 ou32_Flag) const;
   void OpenDetail(const stw_types::sint32 os32_NodeIndex, const stw_types::sint32 os32_DataPoolIndex,
                   const stw_types::sint32 os32_ListIndex, const stw_types::sint32 os32_ElementIndex,
                   const stw_types::sint32 os32_Flag) const;
   stw_types::sintn GetTabIndex(void) const;

   void ImportMessages(void);

   static const stw_types::sintn hsn_TAB_INDEX_PROPERTIES;
   static const stw_types::sintn hsn_TAB_INDEX_COMM;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigChanged(void);
   void SigErrorChange(void);
   void SigNameChanged(const QString & orc_Name, const QString & orc_SubItemName = "",
                       const bool oq_CombineItemAndSubSubName = false);
   void SigSwitchToCoManager(const stw_types::uint32 ou32_NodeIndex, const QString & orc_NodeName,
                             const stw_types::uint8 ou8_InterfaceNumber) const;

protected:
   virtual void showEvent(QShowEvent * const opc_Event) override;

private:
   //Avoid call
   C_SdBueBusEditWidget(const C_SdBueBusEditWidget &);
   C_SdBueBusEditWidget & operator =(const C_SdBueBusEditWidget &);

   void m_DataChanged(void);
   void m_TabChanged(const stw_types::sintn osn_Index);
   void m_CreateTabWidgetsAlways(const stw_types::sintn osn_Index, const bool oq_AdaptCursor);
   void m_CreatePropertiesTab(const bool oq_AdaptCursor);
   void m_CreateCommTab(const bool oq_AdaptCursor);
   static void mh_StartWaitingCursor(void);
   static void mh_EndWaitingCursor(void);

   Ui::C_SdBueBusEditWidget * mpc_Ui;
   const stw_types::uint32 mu32_BusIndex;
   bool mq_DataChanged;
   stw_opensyde_core::C_OSCSystemBus::E_Type me_BusType;
   bool mq_SkipLoadUserSettings;
   C_SdBueBusEditPropertiesWidget * mpc_BusPropertiesWidget;
   C_SdBueComIfDescriptionWidget * mpc_WidgetComIfDescr;
};
}

#endif
