//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base class for common settings bar functionalities

   Base class for common settings bar functionalities

   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_CamOgeWiSettingsBase.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamOgeWiSettingsBase::C_CamOgeWiSettingsBase(QWidget * const opc_Parent) :
   C_OgeWiOnlyBackground(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten move event.

   Here: Handle popup move.

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamOgeWiSettingsBase::moveEvent(QMoveEvent * const opc_Event)
{
   this->m_PrepareMove();

   C_OgeWiOnlyBackground::moveEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get pop up

   \param[in]  orc_Pair    Pair

   \return
   Pop up
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamMosSectionPopup * C_CamOgeWiSettingsBase::mh_GetPopUp(const QPair<C_CamMosSectionPopup *,
                                                                       C_CamOgePubSettingsAdd *> & orc_Pair)
{
   return orc_Pair.first;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get button

   \param[in]  orc_Pair    Pair

   \return
   Button
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamOgePubSettingsAdd * C_CamOgeWiSettingsBase::mh_GetButton(const QPair<C_CamMosSectionPopup *,
                                                                          C_CamOgePubSettingsAdd *> & orc_Pair)
{
   return orc_Pair.second;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Prepare move
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamOgeWiSettingsBase::m_PrepareMove() const
{
   for (std::vector<QPair<stw::opensyde_gui::C_CamMosSectionPopup *,
                          opensyde_gui_elements::C_CamOgePubSettingsAdd *> >::const_iterator c_ItPair =
           this->mc_Settings.cbegin();
        c_ItPair != this->mc_Settings.cend(); ++c_ItPair)
   {
      if (mh_GetPopUp(*c_ItPair)->isVisible())
      {
         const QPoint c_Point = this->m_GetPopupMovePoint(mh_GetButton(*c_ItPair));
         mh_GetPopUp(*c_ItPair)->DoMove(c_Point);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init settings section

   \param[in,out]  opc_PopUp     Pop up
   \param[in,out]  opc_Button    Button
   \param[in,out]  opc_Parent    Parent
   \param[in]      orc_Icon      Icon
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamOgeWiSettingsBase::m_InitSettingsSection(C_CamMosSectionPopup * const opc_PopUp,
                                                   C_CamOgePubSettingsAdd * const opc_Button,
                                                   const QWidget * const opc_Parent, const QString & orc_Icon)
{
   const QPair<stw::opensyde_gui::C_CamMosSectionPopup *,
               opensyde_gui_elements::C_CamOgePubSettingsAdd *> c_Pair(opc_PopUp, opc_Button);

   // set parents here
   // opc_Parent is a layout and would try to integrate the popup, so we better use its parent.
   opc_PopUp->setParent(opc_Parent->parentWidget());
   // hide popups on start
   opc_PopUp->setVisible(false);
   C_CamOgeWiSettingsBase::mh_InitSettingsButton(*opc_Button, orc_Icon);
   this->mc_Settings.push_back(c_Pair);

   // connect buttons to section popups
   connect(opc_Button, &C_CamOgePubSettingsAdd::toggled, this, &C_CamOgeWiSettingsBase::m_ShowAnyPopup);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init settings button

   \param[in,out]  orc_Button    Button
   \param[in]      orc_Icon      Icon
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamOgeWiSettingsBase::mh_InitSettingsButton(C_CamOgePubSettingsAdd & orc_Button, const QString & orc_Icon)
{
   orc_Button.setIconSize(QSize(16, 16));
   orc_Button.setIcon(QIcon(orc_Icon));
   orc_Button.setText("");

   // button checked = popup open
   orc_Button.setCheckable(true);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Show generic popup.

   \param[in]  oq_Checked  Checked
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamOgeWiSettingsBase::m_ShowAnyPopup(const bool oq_Checked)
{
   if (oq_Checked == true)
   {
      for (std::vector<QPair<stw::opensyde_gui::C_CamMosSectionPopup *,
                             opensyde_gui_elements::C_CamOgePubSettingsAdd *> >::const_iterator c_ItPair =
              this->mc_Settings.cbegin();
           c_ItPair != this->mc_Settings.cend(); ++c_ItPair)
      {
         if (mh_GetButton(*c_ItPair) == this->sender())
         {
            this->m_ShowPopup(mh_GetPopUp(*c_ItPair), mh_GetButton(*c_ItPair));
         }
         else
         {
            // hide all other popups
            mh_GetButton(*c_ItPair)->setChecked(false);
         }
      }
   }
   else
   {
      for (std::vector<QPair<stw::opensyde_gui::C_CamMosSectionPopup *,
                             opensyde_gui_elements::C_CamOgePubSettingsAdd *> >::const_iterator c_ItPair =
              this->mc_Settings.cbegin();
           c_ItPair != this->mc_Settings.cend(); ++c_ItPair)
      {
         if (mh_GetButton(*c_ItPair) == this->sender())
         {
            mh_GetPopUp(*c_ItPair)->setVisible(false);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Show a popup

   \param[in]  opc_Popup   Specific popup widget
   \param[in]  opc_Button  Specific button as start point
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamOgeWiSettingsBase::m_ShowPopup(C_CamMosSectionPopup * const opc_Popup,
                                         const C_CamOgePubSettingsAdd * const opc_Button) const
{
   const QPoint c_Point = this->m_GetPopupMovePoint(opc_Button);

   opc_Popup->Show(c_Point);
   //lint -e{429}  no memory leak because opc_Popup points to instances which are freed in destructor
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the point for the DoMove function of C_CamMosSectionPopup

   \param[in]  opc_Button  Specific button as start point

   \return
   Point for DoMove function on parent of splitter widget
*/
//----------------------------------------------------------------------------------------------------------------------
QPoint C_CamOgeWiSettingsBase::m_GetPopupMovePoint(const C_CamOgePubSettingsAdd * const opc_Button) const
{
   // Position of button in C_CamMosWidget widget
   const QPoint c_Settings = opc_Button->mapToParent(opc_Button->rect().topLeft());
   // Position on splitter level
   const QPoint c_Splitter = this->mapToParent(c_Settings);
   // Position on parent of splitter
   const QPoint c_TopWidget = this->parentWidget()->mapToParent(c_Splitter);

   return c_TopWidget;
}
