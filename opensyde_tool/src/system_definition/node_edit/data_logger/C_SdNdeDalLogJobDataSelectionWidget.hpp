//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base widget for log job data selection
   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDALLOGJOBDATASELECTIONWIDGET_HPP
#define C_SDNDEDALLOGJOBDATASELECTIONWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "C_OscDataLoggerDataElementReference.hpp"
#include "C_OgeContextMenu.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeDalLogJobDataSelectionWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDalLogJobDataSelectionWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeDalLogJobDataSelectionWidget(QWidget * const opc_Parent = NULL);
   ~C_SdNdeDalLogJobDataSelectionWidget(void) override;

   void LoadUserSettings(void) const;
   void SaveUserSettings(void) const;
   void SetNodeDataLoggerJob(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataLoggerJobIndex);
   void InitStaticNames(void) const;
   void ReloadDataElements(void);

private:
   void m_AddClicked(void);
   void m_UpdateUi() const;
   void m_UpdateSelection() const;
   void m_LoadDataElements();
   void m_OnDataChangedInModel(const QModelIndex orc_Index, const QString oc_Data);
   void m_UpdateCustomLoggingName(const uint32_t ou32_Index, const QString oc_Data, const bool oq_UseCustomName);
   void m_SetupContextMenu(void);
   void m_OnCustomContextMenuRequested(const QPoint & orc_Pos);
   void m_DeleteSelectedDataElements(void);

   Ui::C_SdNdeDalLogJobDataSelectionWidget * mpc_Ui;
   stw::opensyde_gui_elements::C_OgeContextMenu * mpc_ContextMenu;

   uint32_t mu32_NodeIndex;
   uint32_t mu32_DataLoggerJobIndex;
   std::vector<stw::opensyde_core::C_OscDataLoggerDataElementReference> mc_DataElements;

   //Avoid call
   C_SdNdeDalLogJobDataSelectionWidget(const C_SdNdeDalLogJobDataSelectionWidget &);
   C_SdNdeDalLogJobDataSelectionWidget & operator =(const C_SdNdeDalLogJobDataSelectionWidget &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
