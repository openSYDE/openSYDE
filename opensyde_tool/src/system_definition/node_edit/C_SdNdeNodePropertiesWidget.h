//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Properties dialog for node properties (header)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDENODEPROPERTIESWIDGET_H
#define C_SDNDENODEPROPERTIESWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include <QTimer>

#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeNodePropertiesWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SdNdeNodePropertiesWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeNodePropertiesWidget(QWidget * const opc_Parent = NULL);
   virtual ~C_SdNdeNodePropertiesWidget(void);

   void InitStaticNames(void) const;
   void SelectName(void) const;
   void SetNodeId(const stw_types::uint32 ou32_NodeIndex);
   void SaveToData(void) const;

private:
   //Avoid call
   C_SdNdeNodePropertiesWidget(const C_SdNdeNodePropertiesWidget &);
   C_SdNdeNodePropertiesWidget & operator =(const C_SdNdeNodePropertiesWidget &);

   Ui::C_SdNdeNodePropertiesWidget * mpc_Ui;
   stw_types::uint32 mu32_NodeIndex;
   QTimer mc_Timer;
   stw_types::uint32 mu32_BusIndex;
   QString mc_BusName;
   static const stw_types::sint32 mhs32_PR_INDEX_DISABLED;
   static const stw_types::sint32 mhs32_PR_INDEX_ENABLED;

   void m_SupportedProtocolChange(void);
   void m_CheckNodeName(void);
   void m_TrimNodeName(void) const;
   void m_RegisterChange(void);
   void m_RegisterNameChange(void);
   void m_CheckComInterface(const stw_types::uint32 ou32_Row, const stw_types::uint32 ou32_Column) const;
   void m_GetInterfaceStatus(const stw_types::uint32 ou32_NodeIndex, const stw_types::sintn osn_InterfaceIndex,
                             const stw_types::uint8 ou8_NodeId, const std::vector<stw_types::sint32> & orc_Ip,
                             bool & orq_IdValid, bool & orq_IpValid) const;
   void m_HandlePropertyConflict(const stw_types::sintn osn_Row, const stw_types::sintn osn_ColumnId,
                                 const stw_types::sintn osn_ColumnIp, const bool oq_IdValid,
                                 const bool oq_IpValid) const;
   void m_HandleErrorFeedback(const stw_types::sintn osn_InterfaceIndex, const bool oq_IdValid,
                              const bool oq_IpValid) const;
   void m_HandleCellClick(const stw_types::uint32 ou32_Row, const stw_types::uint32 ou32_Column);
   void m_IpAddressClick(const stw_types::uint32 ou32_Row);
   void m_BusBitrateClick(const stw_types::uint32 ou32_Row, const stw_types::uint32 ou32_Column);
   void m_OpenBus(void);
   void m_LoadFromData(void);
   void m_FlashloaderOptions(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigChanged(void);
   void SigNameChanged(const QString & orc_Name, const QString & orc_SubItemName,
                       const bool oq_CombineItemAndSubSubName);
   void SigBusBitrateClicked(const stw_types::uint32 & oru32_BusIndex, const QString & orc_BusName);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
