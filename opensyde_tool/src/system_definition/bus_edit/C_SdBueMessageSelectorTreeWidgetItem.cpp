//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Message selector tree widget item class (implementation)

   Message selector tree widget item class

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_SdBueMessageSelectorTreeWidgetItem.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;

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
C_SdBueMessageSelectorTreeWidgetItem::C_SdBueMessageSelectorTreeWidgetItem(const bool & orq_IsMessage) :
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
void C_SdBueMessageSelectorTreeWidgetItem::SetError(const bool & orq_Value)
{
   this->mq_HasError = orq_Value;
   m_UpdateIcon();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set always active state

   \param[in] orq_Value New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidgetItem::SetAlwaysActive(const bool & orq_Value)
{
   this->mq_IsAlwaysActive = orq_Value;
   m_UpdateIcon();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sends the signal in case of a changed check state of the message

   \param[in]  os32_Column  Column
   \param[in]  os32_Role    Data role
   \param[in]  orc_Value   New data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidgetItem::setData(const int32_t os32_Column, const int32_t os32_Role,
                                                   const QVariant & orc_Value)
{
   QTreeWidgetItem::setData(os32_Column, os32_Role, orc_Value);

   if ((this->mq_IsMessage == true) &&
       (os32_Role == static_cast<int32_t>(Qt::CheckStateRole)))
   {
      Q_EMIT (this->SigCheckedStateChanged(this));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update current icon
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorTreeWidgetItem::m_UpdateIcon(void)
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
