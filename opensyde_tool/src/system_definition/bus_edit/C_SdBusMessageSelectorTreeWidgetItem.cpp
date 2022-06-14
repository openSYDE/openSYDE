//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       short description (implementation)

   detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_SdBusMessageSelectorTreeWidgetItem.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in] orq_IsMessage Flag is message
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBusMessageSelectorTreeWidgetItem::C_SdBusMessageSelectorTreeWidgetItem(const bool & orq_IsMessage) :
   QTreeWidgetItem(),
   mq_IsMessage(orq_IsMessage),
   mq_HasError(false),
   mq_IsAlwaysActive(false)
{
   QPixmap c_Img;

   //Default icons

   c_Img.load(":images/system_definition/IconMessageInactive.svg");
   this->mc_IconMessage.addPixmap(c_Img);

   c_Img.load(":images/system_definition/IconMessage.svg");
   this->mc_IconMessage.addPixmap(c_Img, QIcon::Selected);

   c_Img.load(":images/system_definition/IconMessage.svg");
   this->mc_IconMessageAlwaysActive.addPixmap(c_Img);

   c_Img.load(":images/system_definition/IconSignalInactive.svg");
   this->mc_IconSignal.addPixmap(c_Img);

   c_Img.load(":images/system_definition/IconSignal.svg");
   this->mc_IconSignal.addPixmap(c_Img, QIcon::Selected);

   //Error icons
   // now only one error state (in contrast to previous active<->inactive),
   // but kept redundant code for easy change to 2 error states

   c_Img.load(":images/system_definition/IconMessageError.svg"); //IconMessageInactiveError
   this->mc_IconMessageError.addPixmap(c_Img);

   c_Img.load(":images/system_definition/IconMessageError.svg"); //IconMessageActiveError
   this->mc_IconMessageError.addPixmap(c_Img, QIcon::Selected);

   c_Img.load(":images/system_definition/IconMessageError.svg"); //IconMessageActiveError
   this->mc_IconMessageAlwaysActiveError.addPixmap(c_Img);

   c_Img.load(":images/system_definition/IconSignalError.svg"); //IconSignalInactiveError
   this->mc_IconSignalError.addPixmap(c_Img);

   c_Img.load(":images/system_definition/IconSignalError.svg"); //IconSignalActiveError
   this->mc_IconSignalError.addPixmap(c_Img, QIcon::Selected);

   m_UpdateIcon();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set error state

   \param[in] orq_Value New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBusMessageSelectorTreeWidgetItem::SetError(const bool & orq_Value)
{
   this->mq_HasError = orq_Value;
   m_UpdateIcon();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set always active state

   \param[in] orq_Value New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBusMessageSelectorTreeWidgetItem::SetAlwaysActive(const bool & orq_Value)
{
   this->mq_IsAlwaysActive = orq_Value;
   m_UpdateIcon();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sends the signal in case of a changed check state of the message

   \param[in]  osn_Column  Column
   \param[in]  osn_Role    Data role
   \param[in]  orc_Value   New data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBusMessageSelectorTreeWidgetItem::setData(const sintn osn_Column, const sintn osn_Role,
                                                   const QVariant & orc_Value)
{
   QTreeWidgetItem::setData(osn_Column, osn_Role, orc_Value);

   if ((this->mq_IsMessage == true) &&
       (osn_Role == static_cast<sintn>(Qt::CheckStateRole)))
   {
      Q_EMIT (this->SigCheckedStateChanged(this));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update current icon
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBusMessageSelectorTreeWidgetItem::m_UpdateIcon(void)
{
   if (this->mq_IsMessage == true)
   {
      if (this->mq_IsAlwaysActive == true)
      {
         if (this->mq_HasError == true)
         {
            this->setIcon(0, this->mc_IconMessageAlwaysActiveError);
         }
         else
         {
            this->setIcon(0, this->mc_IconMessageAlwaysActive);
         }
      }
      else
      {
         if (this->mq_HasError == true)
         {
            this->setIcon(0, this->mc_IconMessageError);
         }
         else
         {
            this->setIcon(0, this->mc_IconMessage);
         }
      }
   }
   else
   {
      if (this->mq_HasError == true)
      {
         this->setIcon(0, this->mc_IconSignalError);
      }
      else
      {
         this->setIcon(0, this->mc_IconSignal);
      }
   }
}
