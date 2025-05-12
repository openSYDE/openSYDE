//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Properties dialog for node properties (header)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDENODEPROPERTIESWIDGET_HPP
#define C_SDNDENODEPROPERTIESWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include <QTimer>

#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeNodePropertiesWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SdNdeNodePropertiesWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeNodePropertiesWidget(QWidget * const opc_Parent = NULL);
   ~C_SdNdeNodePropertiesWidget(void) override;

   void InitStaticNames(void) const;
   void SelectName(void) const;
   void SetNodeId(const uint32_t ou32_NodeIndex);
   void SaveToData(void);

private:
   //Avoid call
   C_SdNdeNodePropertiesWidget(const C_SdNdeNodePropertiesWidget &);
   C_SdNdeNodePropertiesWidget & operator =(const C_SdNdeNodePropertiesWidget &) &;

   Ui::C_SdNdeNodePropertiesWidget * mpc_Ui;
   uint32_t mu32_NodeIndex;
   QTimer mc_Timer;
   uint32_t mu32_BusIndex;
   QString mc_BusName;
   static const int32_t mhs32_PR_INDEX_DISABLED;
   static const int32_t mhs32_PR_INDEX_ENABLED;

   void m_SupportedProtocolChange(void);
   void m_CheckNodeName(void);
   void m_TrimNodeName(void) const;
   void m_RegisterChange(void);
   void m_RegisterErrorChange(void);
   void m_RegisterNameChange(void);
   void m_CheckComInterface(const uint32_t ou32_Row, const uint32_t ou32_Column) const;
   void m_GetInterfaceStatus(const uint32_t ou32_NodeIndex, const int32_t os32_InterfaceIndex, const uint8_t ou8_NodeId,
                             const std::vector<int32_t> & orc_Ip, bool & orq_IdValid, bool & orq_IpValid) const;
   void m_HandlePropertyConflict(const int32_t os32_Row, const int32_t os32_ColumnId, const int32_t os32_ColumnIp,
                                 const bool oq_IdValid, const bool oq_IpValid) const;
   void m_HandleErrorFeedback(const int32_t os32_InterfaceIndex, const bool oq_IdValid, const bool oq_IpValid) const;
   void m_HandleCellClick(const uint32_t ou32_Row, const uint32_t ou32_Column);
   void m_IpAddressClick(const uint32_t ou32_Row);
   void m_BusBitrateClick(const uint32_t ou32_Row);
   void m_OpenBus(void);
   void m_LoadFromData(void);
   void m_FlashloaderOptions(void) const;
   void m_XappSupportChange(const int32_t os32_Index);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigChanged(void);
   void SigErrorChange(void);
   void SigNameChanged(const QString & orc_Name, const QString & orc_SubItemName,
                       const bool oq_CombineItemAndSubSubName);
   void SigBusBitrateClicked(const uint32_t & oru32_BusIndex, const QString & orc_BusName);
   void SigNodePropChanged(void);
   void SigNodeXappSupportChanged(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
