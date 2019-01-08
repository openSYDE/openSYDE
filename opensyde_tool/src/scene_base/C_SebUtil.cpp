//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Common scene utility functions (implementation)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     20.04.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QFile>
#include <QFileInfo>
#include <QImageReader>

#include "stwtypes.h"
#include "C_SebUtil.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Get highest parent

   \param[in,out] opc_Item Item to get highest parent from

   \return
   Highest parent

   \created     13.10.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
QGraphicsItem * C_SebUtil::h_GetHighestParent(QGraphicsItem * const opc_Item)
{
   QGraphicsItem * pc_Parent;
   QGraphicsItem * pc_ReturnItem;

   if (opc_Item == NULL)
   {
      return NULL;
   }

   pc_Parent = opc_Item->parentItem();

   if (pc_Parent == NULL)
   {
      // no parent, return the original item
      pc_ReturnItem = opc_Item;
   }
   else
   {
      // check if the parent has a parent itself
      pc_ReturnItem = h_GetHighestParent(pc_Parent);
   }

   return pc_ReturnItem;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get highest parent

   \param[in,out] opc_Item Item to get highest parent from

   \return
   Highest parent

   \created     13.10.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
const QGraphicsItem * C_SebUtil::h_GetHighestParent(const QGraphicsItem * const opc_Item)
{
   const QGraphicsItem * pc_Parent;
   const QGraphicsItem * pc_ReturnItem;

   if (opc_Item == NULL)
   {
      return NULL;
   }

   pc_Parent = opc_Item->parentItem();

   if (pc_Parent == NULL)
   {
      // no parent, return the original item
      pc_ReturnItem = opc_Item;
   }
   else
   {
      // check if the parent has a parent itself
      pc_ReturnItem = h_GetHighestParent(pc_Parent);
   }

   return pc_ReturnItem;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Checks the file for existence of the file and file format

   Supported image formats:
   - jpg
   - jpeg
   - png
   - bmp

   \param[in]     orc_FilePath   Path of file

   \return
   true     File exist and is an image

   \created     08.11.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
bool C_SebUtil::h_CheckFilePathForImage(QString & orc_FilePath)
{
   bool q_Return = false;

   // in case of drag and drop, remove the file prefix
   orc_FilePath = orc_FilePath.remove("file:///");

   // check if the file exists and has the correct file type
   if (QFile::exists(orc_FilePath) == true)
   {
      const QFileInfo c_File(orc_FilePath);
      //Manually prevent accepting openSYDE files as image
      if (c_File.completeSuffix().compare("syde") != 0)
      {
         const QImageReader c_ImageReader(orc_FilePath);
         if (c_ImageReader.canRead() == true)
         {
            q_Return = true;
         }
      }
   }

   return q_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Add offset for line item

   \param[in] orc_LineBase Line base item to add offset for
   \param[in] orc_Offset   Offset for all items

   \created     14.11.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SebUtil::h_AddLineOffset(C_PuiBsLineBase & orc_LineBase, const QPointF & orc_Offset)
{
   for (uint32 u32_ItElem = 0; u32_ItElem < orc_LineBase.c_UIInteractionPoints.size(); ++u32_ItElem)
   {
      orc_LineBase.c_UIInteractionPoints[u32_ItElem] += orc_Offset;
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \created     20.04.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SebUtil::C_SebUtil()
{
}
