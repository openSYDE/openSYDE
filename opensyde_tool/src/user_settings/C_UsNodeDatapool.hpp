//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Project node datapool user settings (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_USNODEDATAPOOL_H
#define C_USNODEDATAPOOL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>
#include <QMap>
#include <QString>
#include "C_UsCommunication.hpp"
#include "C_UsNodeDatapoolList.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_UsNodeDatapool
{
public:
   C_UsNodeDatapool(void);

   const std::vector<QString> & GetExpandedListNames(void) const;
   void SetExpandedListNames(const std::vector<QString> & orc_Value);

   const std::vector<QString> & GetSelectedListNames() const;
   void SetSelectedListNames(const std::vector<QString> & orc_Value);

   const std::vector<QString> & GetSelectedVariableNames(void) const;
   void SetSelectedVariableNames(const std::vector<QString> & orc_Value);

   const QList<QString> GetInterfaceSettingsKeysInternal(void) const;
   C_UsCommunication GetCommList(const QString & orc_ListName) const;
   void SetCommMessageOverviewColumnWidth(const QString & orc_ListName, const std::vector<int32_t> & orc_Value);
   void SetCommSignalOverviewColumnWidth(const QString & orc_ListName, const std::vector<int32_t> & orc_Value);
   void SetListSelectedMessage(const QString & orc_ListName,
                               const stw::opensyde_core::C_OscCanProtocol::E_Type oe_SelectedProtocol,
                               const bool oq_MessageSelected, const QString & orc_SelectedMessageName,
                               const bool oq_SignalSelected, const QString & orc_SelectedSignalName);

   const QList<QString> GetListSettingsKeysInternal(void) const;
   C_UsNodeDatapoolList GetOtherList(const QString & orc_ListName) const;
   void SetListColumnSizes(const QString & orc_ListName, const std::vector<int32_t> & orc_ColumnWidths);

private:
   std::vector<QString> mc_ExpandedListNames;
   std::vector<QString> mc_SelectedListNames;
   std::vector<QString> mc_SelectedVariableNames;
   QMap<QString, C_UsCommunication> mc_InterfaceSettings;
   QMap<QString, C_UsNodeDatapoolList> mc_ListSettings;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
