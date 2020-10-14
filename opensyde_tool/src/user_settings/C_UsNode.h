//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Project node user settings (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_USNODE_H
#define C_USNODE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QString>
#include <QMap>
#include "C_UsNodeDatapool.h"
#include "C_OSCCanProtocol.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_UsNode
{
public:
   C_UsNode(void);

   C_UsNodeDatapool GetDatapool(const QString & orc_DatapoolName) const;
   const QList<QString> GetDatapoolKeysInternal(void) const;
   QString GetSelectedDatapoolName(void) const;
   stw_opensyde_core::C_OSCCanProtocol::E_Type GetSelectedProtocol(void) const;
   const std::vector<stw_types::sint32> & GetHalcOverviewColumnWidth(void) const;
   const std::vector<stw_types::sint32> & GetHalcConfigColumnWidth(void) const;
   QString GetSelectedHalcDomainName(void) const;
   QString GetSelectedHalcChannel(void) const;

   void SetSelectedDatapoolName(const QString & orc_DatapoolName);
   void SetSelectedProtocol(const stw_opensyde_core::C_OSCCanProtocol::E_Type oe_Protocol);
   void SetDatapoolOpenListNames(const QString & orc_DatapoolName, const std::vector<QString> & orc_New);
   void SetDatapoolSelectedListNames(const QString & orc_DatapoolName, const std::vector<QString> & orc_New);
   void SetDatapoolSelectedVariableNames(const QString & orc_DatapoolName, const std::vector<QString> & orc_New);
   void SetDatapoolCommMessageOverviewColumnWidth(const QString & orc_DatapoolName, const QString & orc_ListName,
                                                  const std::vector<stw_types::sint32> & orc_Value);
   void SetDatapoolCommSignalOverviewColumnWidth(const QString & orc_DatapoolName, const QString & orc_ListName,
                                                 const std::vector<stw_types::sint32> & orc_Value);
   void SetDatapoolListSelectedMessage(const QString & orc_DatapoolName, const QString & orc_ListName,
                                       const stw_opensyde_core::C_OSCCanProtocol::E_Type oe_SelectedProtocol,
                                       const bool oq_MessageSelected, const QString & orc_SelectedMessageName,
                                       const bool oq_SignalSelected, const QString & orc_SelectedSignalName);
   void SetDatapoolListColumnSizes(const QString & orc_DatapoolName, const QString & orc_ListName,
                                   const std::vector<stw_types::sint32> & orc_ColumnWidths);
   void SetHalcOverviewColumnWidth(const std::vector<stw_types::sint32> & orc_Value);
   void SetHalcConfigColumnWidth(const std::vector<stw_types::sint32> & orc_Value);
   void SetSelectedHalcDomain(const QString & orc_Value);
   void SetSelectedHalcChannel(const QString & orc_Value);

private:
   std::vector<stw_types::sint32> mc_HalcOverviewColumnWidth;
   std::vector<stw_types::sint32> mc_HalcConfigColumnWidth;
   QString mc_SelectedDataPoolName;
   QString mc_SelectedHalcDomainName;
   QString mc_SelectedHalcChannelId;
   stw_opensyde_core::C_OSCCanProtocol::E_Type me_SelectedProtocol;
   QMap<QString, C_UsNodeDatapool> mc_Datapools; ///< History of last known node datapool user settings
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
