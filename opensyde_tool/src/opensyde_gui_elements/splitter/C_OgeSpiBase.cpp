//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Common splitter functionality (implementation)

   Common splitter functionality

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_OgeSpiBase.h"

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
C_OgeSpiBase::C_OgeSpiBase(QWidget * const opc_Parent) :
   QSplitter(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set width for first segment

   \param[in] os32_Width Desired width of first segment
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSpiBase::SetFirstSegment(const sint32 os32_Width)
{
   const QList<sintn> & rc_CurList = this->sizes();

   if (rc_CurList.size() == 2)
   {
      QList<sintn> c_Segments;

      // Adapt the size of both sides
      c_Segments.push_back(static_cast<sintn>(os32_Width));
      // calculation:
      // new size of second segment =
      //     = total size of both segments - new size of first segment =
      //     = (old size of second segment + old size of first segment) - new size of second segment
      c_Segments.push_back(static_cast<sintn>((rc_CurList.at(1) + rc_CurList.at(0)) - c_Segments.at(0)));

      this->setSizes(c_Segments);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set width for second segment

   \param[in] os32_Width Desired width of second segment
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSpiBase::SetSecondSegment(const sint32 os32_Width)
{
   const QList<sintn> & rc_CurList = this->sizes();

   if (rc_CurList.size() == 2)
   {
      QList<sintn> c_Segments;

      // Adapt the size of both sides
      c_Segments.push_front(static_cast<sintn>(os32_Width));
      // calculation:
      // new size of first segment =
      //     = total size of both segments - new size of second segment =
      //     = (old size of second segment + old size of first segment) - new size of second segment
      c_Segments.push_front(static_cast<sintn>((rc_CurList.at(1) + rc_CurList.at(0)) - c_Segments.at(0)));

      this->setSizes(c_Segments);
   }
}
