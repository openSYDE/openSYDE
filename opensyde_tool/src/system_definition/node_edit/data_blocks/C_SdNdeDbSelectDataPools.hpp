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
#include "stwtypes.hpp"
#include "C_OgePopUpDialog.hpp"
#include "C_PuiSvDbNodeDataPoolListElementId.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeDbSelectDataPools;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDbSelectDataPools :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeDbSelectDataPools(const uint32_t ou32_NodeIndex, const int32_t os32_SkipApplicationIndex,
                                     const std::vector<uint32_t> & orc_UsedDataPoolIndicesIndex,
                                     stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent);
   ~C_SdNdeDbSelectDataPools(void) override;

   void InitStaticNames(void) const;
   std::vector<stw::opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId> GetSelectedDataPools(void) const;

protected:
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   Ui::C_SdNdeDbSelectDataPools * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw::opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;
   const uint32_t mu32_NodeIndex;
   const std::vector<uint32_t> mc_UsedDataPoolIndices;

   void m_OkClicked(void);
   void m_CancelClicked(void);

   void m_HandleHiding(void) const;
   void m_OnSearch(const QString & orc_Text) const;
   void m_UpdateSelection(const int32_t os32_SelectionCount) const;
   bool m_IsCommDatapoolSelectionValid(void);

   //Avoid call
   C_SdNdeDbSelectDataPools(const C_SdNdeDbSelectDataPools &);
   C_SdNdeDbSelectDataPools & operator =(const C_SdNdeDbSelectDataPools &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
