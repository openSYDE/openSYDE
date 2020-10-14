//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Combo box with base functionality for table usage

   Combo box with base functionality for table usage

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_OgeCbxTableBase.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeCbxTableBase::C_OgeCbxTableBase(QWidget * const opc_Parent) :
   C_OgeCbxResizingView(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize the values from two string lists

   \param[in] orc_Strings Strings to display
   \param[in] orc_Values  Values to use
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeCbxTableBase::InitFromStringList(const QStringList & orc_Strings, const QStringList & orc_Values)
{
   this->mc_Values = orc_Values;
   this->mc_Strings = orc_Strings;
   for (sintn sn_It = 0; sn_It < orc_Strings.size(); ++sn_It)
   {
      this->addItem(orc_Strings.at(sn_It));
   }
   //Default fallback
   if (this->mc_Values.size() != this->mc_Strings.size())
   {
      this->mc_Values.clear();
      this->mc_Values.reserve(this->mc_Strings.size());
      for (sintn sn_It = 0; sn_It < this->mc_Strings.size(); ++sn_It)
      {
         this->mc_Values.push_back(QString::number(sn_It));
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get current index as value

   \return
   Current index as value
*/
//----------------------------------------------------------------------------------------------------------------------
sint64 C_OgeCbxTableBase::GetValueForCurrentIndex(void) const
{
   sint64 s64_Retval = this->currentIndex();

   if ((s64_Retval < this->mc_Values.size()) && (s64_Retval >= 0))
   {
      bool q_Ok;
      const QString & rc_CurVal = this->mc_Values.at(static_cast<sint32>(s64_Retval));
      const sint64 s64_Tmp = rc_CurVal.toLongLong(&q_Ok);
      if (q_Ok)
      {
         s64_Retval = s64_Tmp;
      }
   }
   return s64_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set the current index based on the specified value

   \param[in] os64_Value Value to set
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeCbxTableBase::SetValue(const sint64 os64_Value)
{
   bool q_Found = false;
   const QString c_Search = QString::number(os64_Value);

   for (sintn sn_It = 0; sn_It < this->mc_Values.size(); ++sn_It)
   {
      if (this->mc_Values.at(sn_It) == c_Search)
      {
         this->setCurrentIndex(sn_It);
         q_Found = true;
      }
   }
   if (q_Found == false)
   {
      this->setCurrentIndex(-1);
   }
}
