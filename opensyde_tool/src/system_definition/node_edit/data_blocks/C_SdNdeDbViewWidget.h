//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing all node applications of a specific node (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDBVIEWWIDGET_H
#define C_SDNDEDBVIEWWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "stwtypes.h"
#include "C_OSCNodeApplication.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeDbViewWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDbViewWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeDbViewWidget(QWidget * const opc_Parent = NULL);
   virtual ~C_SdNdeDbViewWidget();

   void InitStaticNames(void) const;
   void SetNodeIndex(const stw_types::uint32 ou32_NodeIndex);
   void ShowApplication(const stw_types::uint32 ou32_ApplicationIndex) const;
   void AddApp(void);
   void UpdateApplications(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigErrorChange(void);
   void SigOpenDataPool(const stw_types::uint32 ou32_DataPoolIndex);

private:
   //Avoid call
   C_SdNdeDbViewWidget(const C_SdNdeDbViewWidget &);
   C_SdNdeDbViewWidget & operator =(const C_SdNdeDbViewWidget &);

   void m_OnDelete(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_ApplicationIndex);
   void m_OnAppDisplay() const;
   void m_HandleNoDatablocksLabel() const;
   void m_AddFromTSP(void);
   void m_UpdateCount(void) const;
   void m_ProgrammingOptions(void) const;
   void m_AddManualApplication(const stw_opensyde_core::C_OSCNodeApplication::E_Type oe_Type);
   void m_HandleCodeGenerationSettingsButtonAvailability(void) const;
   void m_HandleAddButtonAvailability(void) const;
   stw_types::uint32 m_AddApplication(const stw_opensyde_core::C_OSCNodeApplication::E_Type oe_Type) const;
   stw_types::uint32 m_AddApplication(stw_opensyde_core::C_OSCNodeApplication & orc_Application) const;

   Ui::C_SdNdeDbViewWidget * mpc_Ui;
   stw_types::uint32 mu32_NodeIndex;
   static const QString mhc_DefaultDataBlockName;
};
}

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
#endif
