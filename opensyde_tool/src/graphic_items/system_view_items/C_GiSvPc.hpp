//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Offers visualization and functionality of a PC. (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GIPC_HPP
#define C_GIPC_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"
#include "C_GiImageGroupWithoutData.hpp"
#include "C_GiRectPixmap.hpp"
#include "C_PuiSvDbDataElement.hpp"
#include "C_GiBiCustomToolTip.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiSvPc :
   public C_GiImageGroupWithoutData,
   public C_GiBiCustomToolTip,
   public stw::opensyde_gui_logic::C_PuiSvDbDataElement
{
   Q_OBJECT

public:
   C_GiSvPc(const uint64_t ou64_UniqueId, const uint32_t ou32_ViewIndex);
   ~C_GiSvPc(void) override;

   void FindClosestPoint(const QPointF & orc_ScenePoint, QPointF & orc_Closest) const override;
   int32_t type(void) const override;
   bool OpenDialog(void) const;

   void SetConnected(const bool oq_Connected);

   void LoadData(void) override;
   void UpdateData(void) override;
   void DeleteData(void) override;
   void SetEditMode(const bool oq_EditMode);
   void GenerateHint(void) override;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigHideToolTip(void);

protected:
   void m_ResizeUpdateItems(const float64_t of64_DiffWidth, const float64_t of64_DiffHeight) override;
   void mousePressEvent(QGraphicsSceneMouseEvent * const opc_Event) override;
   void hoverLeaveEvent(QGraphicsSceneHoverEvent * const opc_Event) override;

private:
   bool mq_Connected;
   C_GiRectPixmap * mpc_ConflictIcon;
   int32_t ms32_IconSize;
   static const uint32_t mhu32_SCALE_CATEGORY_0;
   static const uint32_t mhu32_SCALE_CATEGORY_1;
   static const uint32_t mhu32_SCALE_CATEGORY_2;
   static const uint32_t mhu32_SCALE_CATEGORY_3;
   static const float64_t mhaf64_SCALE_MIN_WIDTH_NODE[3];
   static const float64_t mhaf64_SCALE_MIN_HEIGHT_NODE[3];
   static const float64_t mhf64_INIT_SIZE_OF_PC;

   //Avoid call
   C_GiSvPc(const C_GiSvPc &);
   C_GiSvPc & operator =(const C_GiSvPc &) &; //lint !e1511 //we want to hide the base func.

   bool m_OpenCanDllDialog(void) const;

   static bool mh_GetIsLaptop(void);
   void m_InitConflictIcon(void);
   void m_DetectIconSize(void);

   void m_UpdateItems(const float64_t of64_DiffWidth, const float64_t of64_DiffHeight, const bool oq_Initial);
   uint32_t m_GetScaleCategory(void) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
