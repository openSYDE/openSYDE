//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Implementation for drawing element image in system view dashboard (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     20.07.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_GISVDAIMAGEGROUP_H
#define C_GISVDAIMAGEGROUP_H

/* -- Includes ------------------------------------------------------------- */
#include "C_GiBiImageGroup.h"
#include "C_PuiSvDbDataElement.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_GiSvDaImageGroup :
   public C_GiBiImageGroup,
   public stw_opensyde_gui_logic::C_PuiSvDbDataElement
{
public:
   C_GiSvDaImageGroup(const stw_types::uint32 & oru32_ViewIndex, const stw_types::uint32 & oru32_DashboardIndex,
                      const stw_types::sint32 & ors32_DataIndex, const stw_types::uint64 & oru64_ID,
                      const QString & orc_ImagePath, QGraphicsItem * const opc_Parent = NULL);
   C_GiSvDaImageGroup(const stw_types::uint32 & oru32_ViewIndex, const stw_types::uint32 & oru32_DashboardIndex,
                      const stw_types::sint32 & ors32_DataIndex, const stw_types::uint64 & oru64_ID,
                      const stw_types::float64 of64_Width, const stw_types::float64 of64_Height,
                      const QPixmap & orc_Image, QGraphicsItem * const opc_Parent = NULL);
   virtual void LoadData(void) override;
   virtual void UpdateData(void) override;
   virtual void DeleteData(void) override;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
