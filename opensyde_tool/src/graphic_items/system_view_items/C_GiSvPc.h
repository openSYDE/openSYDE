//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Offers visualization and functionality of a PC. (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GIPC_H
#define C_GIPC_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"
#include "C_GiImageGroupWithoutData.h"
#include "C_GiRectPixmap.h"
#include "C_PuiSvDbDataElement.h"
#include "C_GiBiCustomToolTip.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiSvPc :
   public C_GiImageGroupWithoutData,
   public C_GiBiCustomToolTip,
   public stw_opensyde_gui_logic::C_PuiSvDbDataElement
{
   Q_OBJECT

public:
   C_GiSvPc(const stw_types::uint64 ou64_UniqueID, const stw_types::uint32 ou32_ViewIndex);
   virtual ~C_GiSvPc(void);

   virtual void FindClosestPoint(const QPointF & orc_ScenePoint, QPointF & orc_Closest) const;
   virtual stw_types::sintn type(void) const override;
   bool OpenDialog(void) const;

   void SetConnected(const bool oq_Connected);

   virtual void LoadData(void) override;
   virtual void UpdateData(void) override;
   virtual void DeleteData(void) override;
   void SetEditMode(const bool oq_EditMode);
   virtual void GenerateHint(void) override;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigHideToolTip(void);

protected:
   virtual void m_ResizeUpdateItems(const stw_types::float64 of64_DiffWidth,
                                    const stw_types::float64 of64_DiffHeight) override;
   virtual void mousePressEvent(QGraphicsSceneMouseEvent * const opc_Event) override;
   virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent * const opc_Event) override;

private:
   bool mq_Connected;
   C_GiRectPixmap * mpc_ConflictIcon;
   stw_types::sint32 ms32_IconSize;
   static const stw_types::uint32 mhu32_ScaleCategory0;
   static const stw_types::uint32 mhu32_ScaleCategory1;
   static const stw_types::uint32 mhu32_ScaleCategory2;
   static const stw_types::uint32 mhu32_ScaleCategory3;
   static const stw_types::float64 mhaf64_ScaleMinWidthNode[3];
   static const stw_types::float64 mhaf64_ScaleMinHeightNode[3];
   static const stw_types::float64 mhf64_InitSizeOfPc;

   //Avoid call
   C_GiSvPc(const C_GiSvPc &);
   C_GiSvPc & operator =(const C_GiSvPc &); //lint !e1511 //we want to hide the base func.

   bool m_OpenCANDllDialog(void) const;

   static bool mh_GetIsLaptop(void);
   void m_InitConflictIcon(void);
   void m_DetectIconSize(void);

   void m_UpdateItems(const stw_types::float64 of64_DiffWidth, const stw_types::float64 of64_DiffHeight,
                      const bool oq_Initial);
   stw_types::uint32 m_GetScaleCategory(void) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
