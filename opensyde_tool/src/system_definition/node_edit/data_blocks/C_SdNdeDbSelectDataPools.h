//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget to handle data pool selections for data blocks (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDBSELECTDATAPOOLS_H
#define C_SDNDEDBSELECTDATAPOOLS_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "stwtypes.h"
#include "C_OgePopUpDialog.h"
#include "C_PuiSvDbNodeDataPoolListElementId.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeDbSelectDataPools;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDbSelectDataPools :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeDbSelectDataPools(const stw_types::uint32 ou32_NodeIndex,
                                     const stw_types::sint32 os32_SkipApplicationIndex,
                                     const std::vector<stw_types::uint32> & orc_UsedDataPoolIndicesIndex,
                                     stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent);
   ~C_SdNdeDbSelectDataPools(void);

   void InitStaticNames(void) const;
   std::vector<stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId> GetSelectedDataPools(void) const;

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;
   //lint -restore

private:
   Ui::C_SdNdeDbSelectDataPools * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw_opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;
   const stw_types::uint32 mu32_NodeIndex;
   const std::vector<stw_types::uint32> mc_UsedDataPoolIndices;

   void m_OkClicked(void);
   void m_CancelClicked(void);

   void m_HandleHiding(void) const;
   void m_OnSearch(const QString & orc_Text) const;
   void m_UpdateSelection(const stw_types::sintn osn_SelectionCount) const;
   bool m_IsCommDatapoolSelectionValid(void);

   //Avoid call
   C_SdNdeDbSelectDataPools(const C_SdNdeDbSelectDataPools &);
   C_SdNdeDbSelectDataPools & operator =(const C_SdNdeDbSelectDataPools &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
