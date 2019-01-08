//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Implementation for drawing element image in system definition (implementation)

   Implementation for drawing element image in system definition

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     20.07.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_GiSdImageGroup.h"
#include "C_PuiSdHandler.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   Set up GUI with all elements.

   \param[in]     ors32_Index   Index of data element in system definition
   \param[in]     oru64_ID      Unique ID
   \param[in]     orc_ImagePath File path of image
   \param[in,out] opc_Parent    Optional pointer to parent

   \created     20.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_GiSdImageGroup::C_GiSdImageGroup(const sint32 & ors32_Index, const uint64 & oru64_ID, const QString & orc_ImagePath,
                                   QGraphicsItem * const opc_Parent) :
   C_GiBiImageGroup(oru64_ID, orc_ImagePath, opc_Parent),
   C_PuiSdDataElement(ors32_Index, C_PuiSdDataElement::eIMAGE)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   Set up GUI with all elements.

   \param[in]     ors32_Index   Index of data element in system definition
   \param[in]     oru64_ID      Unique ID
   \param[in]     orc_Image     Image
   \param[in]     orf64_Width   Width of image view
   \param[in]     orf64_Height  Height of image view
   \param[in]     orc_ImagePath Filepath to the image
   \param[in,out] opc_Parent    Optional pointer to parent

   \created     20.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_GiSdImageGroup::C_GiSdImageGroup(const sint32 & ors32_Index, const uint64 & oru64_ID, const float64 of64_Width,
                                   const float64 of64_Height, const QPixmap & orc_Image,
                                   QGraphicsItem * const opc_Parent) :
   C_GiBiImageGroup(oru64_ID, of64_Width, of64_Height, orc_Image, opc_Parent),
   C_PuiSdDataElement(ors32_Index, C_PuiSdDataElement::eIMAGE)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Function for initially loading internal data

   \created     10.11.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiSdImageGroup::LoadData(void)
{
   if (C_PuiSdHandler::h_GetInstance()->c_Elements.c_Images.size() > static_cast<uint32>(this->ms32_Index))
   {
      this->LoadBasicData(C_PuiSdHandler::h_GetInstance()->c_Elements.c_Images[static_cast<uint32>(this->ms32_Index)]);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Slot function for updating internal data

   \created     02.11.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_GiSdImageGroup::UpdateData(void)
{
   if (C_PuiSdHandler::h_GetInstance()->c_Elements.c_Images.size() > static_cast<uint32>(this->ms32_Index))
   {
      m_UpdateData(C_PuiSdHandler::h_GetInstance()->c_Elements.c_Images[this->ms32_Index]);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Delete data in system definition

   \created     14.09.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiSdImageGroup::DeleteData(void)
{
   C_PuiSdHandler::h_GetInstance()->c_Elements.c_Images.erase(
      C_PuiSdHandler::h_GetInstance()->c_Elements.c_Images.begin() + ms32_Index);
}
