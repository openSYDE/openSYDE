//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing all node applications of a specific node (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDBVIEWWIDGET_HPP
#define C_SDNDEDBVIEWWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "stwtypes.hpp"
#include "C_OscNodeApplication.hpp"
#include "C_SdNdeDbAddNewProject.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeDbViewWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDbViewWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeDbViewWidget(QWidget * const opc_Parent = NULL);
   ~C_SdNdeDbViewWidget() override;

   void InitStaticNames(void) const;
   void SetNodeIndex(const uint32_t ou32_NodeIndex);
   void ShowApplication(const uint32_t ou32_ApplicationIndex) const;
   void AddApp(void);
   void UpdateApplications(void) const;
   bool AddFromTsp(const bool oq_IsNewNode);
   void AddHalcDefFromTsp(stw::opensyde_gui::C_SdNdeDbAddNewProject * const opc_Dialog);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigErrorChange(void);
   void SigOwnedDataPoolsChanged(void) const;
   void SigHalcLoadedFromTsp(void);
   void SigUpdateTrigger(uint32_t ou32_NodeIndex);

private:
   //Avoid call
   C_SdNdeDbViewWidget(const C_SdNdeDbViewWidget &);
   C_SdNdeDbViewWidget & operator =(const C_SdNdeDbViewWidget &) &;

   void m_OnDelete(const uint32_t ou32_NodeIndex, const uint32_t ou32_ApplicationIndex);
   void m_DeleteAllDatablocks(const uint32_t ou32_NodeIndex,
                              const std::vector<stw::opensyde_core::C_OscNodeApplication> & orc_Applications);
   void m_OnAppDisplay() const;
   void m_HandleNoDatablocksLabel() const;
   void m_UpdateCount(void) const;
   void m_ProgrammingOptions(void) const;
   void m_AddManualApplication(void);
   void m_HandleCodeGenerationSettingsButtonAvailability(void) const;
   void m_HandleAddButtonAvailability(void) const;
   uint32_t m_AddApplication(const stw::opensyde_core::C_OscNodeApplication::E_Type oe_Type) const;
   uint32_t m_AddApplication(stw::opensyde_core::C_OscNodeApplication & orc_Application) const;
   void m_UpdateTrigger();

   Ui::C_SdNdeDbViewWidget * mpc_Ui;
   uint32_t mu32_NodeIndex;
};
}
}

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
#endif
