//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Pop up widget for DP preview
   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEHALCCHANNELDPPREVIEWPOPUP_H
#define C_SDNDEHALCCHANNELDPPREVIEWPOPUP_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "stwtypes.hpp"
#include "C_OgePopUpDialog.hpp"
#include "C_OscHalcDefDomain.hpp"
#include "C_OscHalcMagicianDatapoolListHandler.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeHalcChannelDpPreviewPopUp;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeHalcChannelDpPreviewPopUp :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeHalcChannelDpPreviewPopUp(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                             const uint32_t ou32_NodeIndex, const uint32_t ou32_DomainIndex,
                                             const uint32_t ou32_ChannelIndex, const bool oq_UseChannelIndex);
   ~C_SdNdeHalcChannelDpPreviewPopUp(void) override;

   void InitStaticNames(void) const;

protected:
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   Ui::C_SdNdeHalcChannelDpPreviewPopUp * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw::opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;

   void m_OkClicked(void);

   void m_UpdateContent(const uint32_t ou32_NodeIndex, const uint32_t ou32_DomainIndex,
                        const uint32_t ou32_ChannelIndex, const bool oq_UseChannelIndex) const;
   static void mh_AddDpSection(const uint32_t ou32_NodeIndex, const uint32_t ou32_DomainIndex,
                               const uint32_t ou32_ChannelIndex, const bool oq_UseChannelIndex, QString & orc_Text);
   static void mh_AddDeSection(const uint32_t ou32_NodeIndex, const uint32_t ou32_DomainIndex,
                               const uint32_t ou32_ChannelIndex, const bool oq_UseChannelIndex, QString & orc_Text);
   static void mh_AddUseCase(const uint32_t ou32_NodeIndex, const uint32_t ou32_DomainIndex,
                             const uint32_t ou32_ChannelIndex, const bool oq_UseChannelIndex, QString & orc_Text);
   static void mh_AddListSection(const std::vector<stw::opensyde_core::C_OscHalcDefStruct> & orc_Definition,
                                 const std::vector<uint32_t> & orc_Indices, const stw::scl::C_SclString & orc_ListName,
                                 const stw::scl::C_SclString & orc_DomainSingularName, QString & orc_Text,
                                 const stw::opensyde_core::C_OscHalcDefDomain & orc_Domain,
                                 const stw::opensyde_core::C_OscHalcMagicianDatapoolListHandler & orc_DpHandler,
                                 const bool oq_AddSpecialVars = false);
   static void mh_AddListEntry(const stw::scl::C_SclString & orc_ListName, bool & orq_ListAdded,
                               const stw::scl::C_SclString & orc_VarName, const stw::scl::C_SclString & orc_VarComment,
                               QString & orc_Text);

   //Avoid call
   C_SdNdeHalcChannelDpPreviewPopUp(const C_SdNdeHalcChannelDpPreviewPopUp &);
   C_SdNdeHalcChannelDpPreviewPopUp & operator =(const C_SdNdeHalcChannelDpPreviewPopUp &) &;

   static const int32_t mhs32_TABLE_SPACING;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
