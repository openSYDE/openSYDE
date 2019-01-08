//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Widget for PieChart

   Widget for PieChart

   \implementation
   project     opensyde
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     06.07.2016  STW/S.Singer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_OgeWiPieChart.h"
#include "CSCLString.h"
#include "ui_C_OgeWiPieChart.h"
#include "constants.h"

#include <QPainter>

/* -- Used Namespaces ------------------------------------------------------ */

using namespace stw_types;
using namespace stw_opensyde_gui;

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

   \param[in,out] opc_Parent Optional pointer to parent

   \created     06.07.2016  STW/S.Singer
*/
//-----------------------------------------------------------------------------
C_OgeWiPieChart::C_OgeWiPieChart(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_OgeWiPieChart)
{
   this->mpc_Ui->setupUi(this);

   mu16_ValueInPercent = 0;

   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_SpinBox, static_cast<void (QSpinBox::*)(
                                                    stw_types::sintn)>(&QSpinBox::valueChanged), this,
           &C_OgeWiPieChart::m_OnSpinboxValuechanged);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   \created     06.07.2016  STW/S.Singer
*/
//-----------------------------------------------------------------------------
C_OgeWiPieChart::~C_OgeWiPieChart()
{
   delete mpc_Ui;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overrided paint event

   Draws the element

   \param[in,out] opc_Event  Pointer to paint event

   \created     06.07.2016  STW/S.Singer
*/
//-----------------------------------------------------------------------------
void C_OgeWiPieChart::paintEvent(QPaintEvent * const opc_Event)
{
   QPainter c_Painter(this);

   c_Painter.save();

   QBrush c_BackgroundBrush(QColor(212, 212, 212));
   QBrush c_ForegroundBrush(QColor(245, 247, 248));
   QBrush c_PieBrush(QColor(247, 147, 30));
   QPen c_Pen(QColor(91, 91, 91));
   QFont c_Font;
   QRect c_Rect(50, 50, 200, 200);
   stw_types::sint16 s16_StartAngle;
   stw_types::float32 f32_SpanAngle;
   stw_scl::C_SCLString c_String;

   c_BackgroundBrush.setColor(QColor(2, 155, 136));
   c_ForegroundBrush.setColor(QColor(40, 40, 40));
   c_PieBrush.setColor(QColor(230, 0, 0));
   c_Pen.setColor(QColor(176, 6, 176));
   c_Font.setFamily("Segoe UI");
   c_Font.setPointSize(44);

   c_Painter.setPen(Qt::NoPen);
   c_Painter.setRenderHint(QPainter::Antialiasing);

   //background cycle
   c_Painter.setBrush(c_BackgroundBrush);
   c_Painter.drawEllipse(50, 50, 200, 200);

   //pie
   c_Rect.setRect(50, 50, 200, 200);
   s16_StartAngle = 90 * 16;
   f32_SpanAngle = (-1.0F * (360.0F / 100.0F) * static_cast<float32>(mu16_ValueInPercent)  * 16.0F);

   c_Painter.setBrush(c_PieBrush);
   c_Painter.drawPie(c_Rect, s16_StartAngle, static_cast<sintn>(f32_SpanAngle));

   //foreground cycle
   c_Painter.setBrush(c_ForegroundBrush);
   c_Painter.drawEllipse(60, 60, 180, 180);

   //text
   c_Painter.setPen(c_Pen);
   c_Painter.setFont(c_Font);
   c_String = stw_scl::C_SCLString::IntToStr(mu16_ValueInPercent);
   c_String += "%";
   c_Painter.drawText(c_Rect, static_cast<sintn>(Qt::AlignCenter), c_String.c_str());

   c_Painter.restore();
   QWidget::paintEvent(opc_Event);

   // Karsten Anfang
   /*
      // Variablen für allgemeinen Pie -----------------------------------------------
      stw_types::float32 f32_PieWidth = 8.0;                    // Breite des Pi-Streifens
      stw_types::float32 f32_RimWidth = 7.0;                    // Breite des Randes (wird für sie Überlappung im Code um 1
                                                                // erhoet
      stw_types::float32 f32_PieGap = f32_PieWidth / 2.0 + 2.0; // Abstand zum Umrandenden Viereck entrpicht
                                                                // Additionsfaktor
      stw_types::float32 f32_GesamtPixel = 158.0;               // Gesamtgröße der Pie-Vorgabe in Pixeln
      stw_types::float32 f32_DotSize = 8.0;                     // Radius der Umlaufenden Kugel bei Skeuomorph
      QConicalGradient c_Gradient;                              // Für das Pie bei Gradation (STW_2)
      // Rechteck des Innenkreises
      QRect c_InnerCircleRect;
      // Rechteck des den Innenkreis umrandenden Ringes
      QRect c_RimRect;
      // Rechteck des Pies
      QRect c_PieRect;
      //------------------------------------------------------------------------------

      // Zeichnen des farbigen Hintergrundes
      c_Painter.setBrush(QBrush(QColor(mu16_BackgroundRed, mu16_BackgroundGreen, mu16_BackgroundBlue))); // Einstellen der
                                                                                                         // Farbe
      c_Painter.setPen(Qt::NoPen);
      c_Painter.drawRect(this->rect());

      // Holen der eingestellten Anfangswerte
      mc_Style = this->mpc_Ui->pc_comboBox->currentIndex();                                           //combobox current
                                                                                                      // item
      mc_Mode = this->mpc_Ui->pc_comboBox_2->currentIndex();                                          //combobox current
                                                                                                      // item
      mu16_BackgroundRed = static_cast<uint16>(this->mpc_Ui->pc_SpinBox_2->value());                  // Background Color
                                                                                                      // Red Part
      mu16_BackgroundGreen = static_cast<uint16>(this->mpc_Ui->pc_SpinBox_3->value());                // Background Color
                                                                                                      // Green Part
      mu16_BackgroundBlue = static_cast<uint16>(this->mpc_Ui->pc_SpinBox_4->value());                 // Background Color
                                                                                                      // Blue Part
      mu16_BackgroundTransparnecy = static_cast<uint16>(this->mpc_Ui->pc_comboBox_3->currentIndex()); // Pie InnerCircle
                                                                                                      // Transparency

      switch (mc_Style) // Verschiedene Dashboard-Styles
      {
      case 0: // Material (openSYDE)
         // Damit die Kanten der Kreise geglättet werden
         c_Painter.setRenderHint(QPainter::Antialiasing);

         if (mc_Mode == 0) // openSYDE DARK
         {
            f32_PieWidth = 8.0;                    // Breite des umlaufenden Pies
            f32_RimWidth = 8.0;                    // Breite des Randes, welcher den innenkreis umschließt
            f32_PieGap = f32_PieWidth / 2.0 + 2.0; // Hilfsvariable; Additionsfaktor beschreibt Abstand zum den Kreis
                                                   // definierenden Rechteck
            f32_GesamtPixel = 158.0;               // Gesamtgröße des Pies, wird benötigt, um korrekt zu skalieren

            // Setzen der Rechtecke aufgrund der eben gesetzten Variablen------------------------
            m_SetAllPieRects(c_InnerCircleRect, c_RimRect, c_PieRect, f32_PieGap, f32_PieWidth, f32_RimWidth,
                             f32_GesamtPixel, 2.0);

            // Pie--------------------------------------------------------------------------------
            m_SetPie(c_Painter, QBrush(mc_STYLE_GUIDE_COLOR_13), Qt::RoundCap, c_PieRect, f32_PieWidth, f32_GesamtPixel);

            // Außenrand--------------------------------------------------------------------------
            m_SetPieRim(c_Painter, QBrush(mc_STYLE_GUIDE_COLOR_2), c_RimRect, f32_RimWidth, f32_GesamtPixel);

            // Innenkreis-------------------------------------------------------------------------
            if (mu16_BackgroundTransparnecy == 0) // Nicht transparent
            {
               m_SetInnerCircle(c_Painter, QBrush(mc_STYLE_GUIDE_COLOR_52), c_InnerCircleRect);
            }
            else // Transparent
            {
               m_SetInnerCircle(c_Painter, QBrush(QColor(Qt::transparent)), c_InnerCircleRect);
            }

            // Text-------------------------------------------------------------------------------
            m_SetPieText(c_Painter, mc_STYLE_GUIDE_COLOR_0, c_InnerCircleRect);

            c_Painter.restore();
            QWidget::paintEvent(opc_Event);
         }
         else // openSYDE BRIGHT
         {
            f32_PieWidth = 7.0;
            f32_RimWidth = 6.0;
            f32_PieGap = f32_PieWidth / 2.0 + 2.0;
            f32_GesamtPixel = 148.0;

            m_SetAllPieRects(c_InnerCircleRect, c_RimRect, c_PieRect, f32_PieGap, f32_PieWidth, f32_RimWidth,
                             f32_GesamtPixel);
            // Pie--------------------------------------------------------------------------------
            m_SetPie(c_Painter, QBrush(mc_STYLE_GUIDE_COLOR_13), Qt::RoundCap, c_PieRect, f32_PieWidth, f32_GesamtPixel);

            // Außenrand--------------------------------------------------------------------------
            m_SetPieRim(c_Painter, QBrush(mc_STYLE_GUIDE_COLOR_38), c_RimRect, f32_RimWidth, f32_GesamtPixel);

            // Innenkreis-------------------------------------------------------------------------
            if (mu16_BackgroundTransparnecy == 0)
            {
               m_SetInnerCircle(c_Painter, QBrush(mc_STYLE_GUIDE_COLOR_0), c_InnerCircleRect);
            }
            else
            {
               m_SetInnerCircle(c_Painter, QBrush(QColor(Qt::transparent)), c_InnerCircleRect);
            }

            // Text-------------------------------------------------------------------------------
            m_SetPieText(c_Painter, mc_STYLE_GUIDE_COLOR_34, c_InnerCircleRect);

            c_Painter.restore();
            QWidget::paintEvent(opc_Event);
         }
         break;

      case 1: // Flat
         f32_PieWidth = 7.0;
         f32_RimWidth = 5.0;
         f32_PieGap = f32_PieWidth / 2.0 + 2.0;
         f32_GesamtPixel = 148.0;

         m_SetAllPieRects(c_InnerCircleRect, c_RimRect, c_PieRect, f32_PieGap, f32_PieWidth, f32_RimWidth, f32_GesamtPixel,
                          2.0);
         // Damit die Kanten der Kreise geglättet werden
         c_Painter.setRenderHint(QPainter::Antialiasing);

         if (mc_Mode == 0) // Flat DARK
         {
            // Pie--------------------------------------------------------------------------------
            m_SetPie(c_Painter, QBrush(mc_STYLE_GUIDE_COLOR_26), Qt::FlatCap, c_PieRect, f32_PieWidth, f32_GesamtPixel);

            // Außenrand--------------------------------------------------------------------------
            m_SetPieRim(c_Painter, QBrush(mc_STYLE_GUIDE_COLOR_32), c_RimRect, f32_RimWidth, f32_GesamtPixel);

            // Innenkreis-------------------------------------------------------------------------
            m_SetInnerCircle(c_Painter, QBrush(mc_STYLE_GUIDE_COLOR_32), c_InnerCircleRect);

            // Text-------------------------------------------------------------------------------
            m_SetPieText(c_Painter, mc_STYLE_GUIDE_COLOR_0, c_InnerCircleRect);

            c_Painter.restore();
            QWidget::paintEvent(opc_Event);
         }
         else // Flat BRIGHT
         {
            // Pie--------------------------------------------------------------------------------
            m_SetPie(c_Painter, QBrush(mc_STYLE_GUIDE_COLOR_26), Qt::FlatCap, c_PieRect, f32_PieWidth, f32_GesamtPixel);

            // Außenrand--------------------------------------------------------------------------
            m_SetPieRim(c_Painter, QBrush(mc_STYLE_GUIDE_COLOR_10), c_RimRect, f32_RimWidth, f32_GesamtPixel);

            // Innenkreis-------------------------------------------------------------------------
            m_SetInnerCircle(c_Painter, QBrush(mc_STYLE_GUIDE_COLOR_10), c_InnerCircleRect);

            // Text-------------------------------------------------------------------------------
            m_SetPieText(c_Painter, mc_STYLE_GUIDE_COLOR_0, c_InnerCircleRect);

            c_Painter.restore();
            QWidget::paintEvent(opc_Event);
         }
         break;

      case 2: // Skeuomorph
         f32_PieWidth = 4.0;
         f32_RimWidth = 7.0;
         f32_PieGap = f32_PieWidth / 2.0 + 2.0;
         f32_GesamtPixel = 142.0;
         f32_DotSize = 8.0;

         m_SetAllPieRects(c_InnerCircleRect, c_RimRect, c_PieRect, f32_PieGap, f32_PieWidth, f32_RimWidth, f32_GesamtPixel,
                          2.0);
         // Damit die Kanten der Kreise geglättet werden
         c_Painter.setRenderHint(QPainter::Antialiasing);

         if (mc_Mode == 0) // Skeuomorph DARK
         {
            // Fabverlauf des Pie-Charts
            QLinearGradient c_LinearGradient;
            QLinearGradient c_LinearGradient_2;
            c_LinearGradient.setColorAt(0.0, mc_STYLE_GUIDE_COLOR_33); // Farbe: 33
            c_LinearGradient.setColorAt(1.0, mc_STYLE_GUIDE_COLOR_34); // Farbe: 34
            c_LinearGradient.setStart(c_InnerCircleRect.topLeft());
            c_LinearGradient.setFinalStop(c_InnerCircleRect.bottomLeft());
            c_LinearGradient_2.setColorAt(1.0, mc_STYLE_GUIDE_COLOR_33); // Farbe: 33
            c_LinearGradient_2.setColorAt(0.0, mc_STYLE_GUIDE_COLOR_34); // Farbe: 34
            c_LinearGradient_2.setStart(c_RimRect.topLeft());
            c_LinearGradient_2.setFinalStop(c_RimRect.bottomLeft());

            // Pie--------------------------------------------------------------------------------
            m_SetPie(c_Painter, QBrush(mc_STYLE_GUIDE_COLOR_24), Qt::FlatCap, c_PieRect, f32_PieWidth, f32_GesamtPixel);

            // Optional Punkt auf Pie-------------------------------------------------------------
            m_SetPieDot(c_Painter, mc_STYLE_GUIDE_COLOR_24, c_PieRect, f32_DotSize, f32_GesamtPixel);

            // Außenrand--------------------------------------------------------------------------
            m_SetPieRim(c_Painter, c_LinearGradient_2, c_RimRect, f32_RimWidth, f32_GesamtPixel);

            // Innenkreis-------------------------------------------------------------------------
            m_SetInnerCircle(c_Painter, c_LinearGradient, c_InnerCircleRect);

            // Text-------------------------------------------------------------------------------
            m_SetPieText(c_Painter, mc_STYLE_GUIDE_COLOR_0, c_InnerCircleRect);

            c_Painter.restore();
            QWidget::paintEvent(opc_Event);
         }
         else // Skeuomorph BRIGHT
         {
            // Farbverlauf des Pie-Charts
            QLinearGradient c_LinearGradient;
            QLinearGradient c_LinearGradient_2;
            c_LinearGradient.setColorAt(0.0, mc_STYLE_GUIDE_COLOR_37); // Farbe: 37
            c_LinearGradient.setColorAt(1.0, mc_STYLE_GUIDE_COLOR_11); // Farbe: 11
            c_LinearGradient.setStart(c_InnerCircleRect.topLeft());
            c_LinearGradient.setFinalStop(c_InnerCircleRect.bottomLeft());
            c_LinearGradient_2.setColorAt(1.0, mc_STYLE_GUIDE_COLOR_37); // Farbe: 37
            c_LinearGradient_2.setColorAt(0.0, mc_STYLE_GUIDE_COLOR_11); // Farbe: 11
            c_LinearGradient_2.setStart(c_RimRect.topLeft());
            c_LinearGradient_2.setFinalStop(c_RimRect.bottomLeft());

            // Pie--------------------------------------------------------------------------------
            m_SetPie(c_Painter, QBrush(mc_STYLE_GUIDE_COLOR_24), Qt::FlatCap, c_PieRect, f32_PieWidth, f32_GesamtPixel);

            // Optional Punkt auf Pie-------------------------------------------------------------
            m_SetPieDot(c_Painter, mc_STYLE_GUIDE_COLOR_24, c_PieRect, f32_DotSize, f32_GesamtPixel);

            // Außenrand--------------------------------------------------------------------------
            m_SetPieRim(c_Painter, c_LinearGradient_2, c_RimRect, f32_RimWidth, f32_GesamtPixel);

            // Innenkreis-------------------------------------------------------------------------
            m_SetInnerCircle(c_Painter, c_LinearGradient, c_InnerCircleRect);

            // Text-------------------------------------------------------------------------------
            m_SetPieText(c_Painter, mc_STYLE_GUIDE_COLOR_34, c_InnerCircleRect);

            c_Painter.restore();
            QWidget::paintEvent(opc_Event);
         }
         break;

      case 3:              // Gradation (openSyde_2)
         if (mc_Mode == 0) // Gradation DARK
         {
            f32_PieWidth = 7.0;
            f32_RimWidth = 7.0;
            f32_PieGap = f32_PieWidth / 2.0 + 2.0;
            f32_GesamtPixel = 150.0;

            m_SetAllPieRects(c_InnerCircleRect, c_RimRect, c_PieRect, f32_PieGap, f32_PieWidth, f32_RimWidth,
                             f32_GesamtPixel, 3.0);

            // Damit die Kanten der Kreise geglättet werden
            c_Painter.setRenderHint(QPainter::Antialiasing);

            // Pie--------------------------------------------------------------------------------
            c_Gradient.setCenter(c_InnerCircleRect.center());
            c_Gradient.setAngle(90);
            c_Gradient.setColorAt(1.0, mc_STYLE_GUIDE_COLOR_33);                                                       // Farbe:
                                                                                                                       // 33
            c_Gradient.setColorAt((1.0 - static_cast<float64>(mu16_ValueInPercent) / 100.0), mc_STYLE_GUIDE_COLOR_13); // Farbe:
                                                                                                                       // 13
            m_SetPie(c_Painter, c_Gradient, Qt::FlatCap, c_PieRect, f32_PieWidth, f32_GesamtPixel);

            // Außenrand--------------------------------------------------------------------------
            m_SetPieRim(c_Painter, QBrush(mc_STYLE_GUIDE_COLOR_32), c_RimRect, f32_RimWidth, f32_GesamtPixel, 2.0);

            // Innenkreis-------------------------------------------------------------------------
            if (mu16_BackgroundTransparnecy == 0)
            {
               m_SetInnerCircle(c_Painter, QBrush(mc_STYLE_GUIDE_COLOR_52), c_InnerCircleRect);
            }
            else
            {
               m_SetInnerCircle(c_Painter, QBrush(QColor(Qt::transparent)), c_InnerCircleRect);
            }

            // Text-------------------------------------------------------------------------------
            m_SetPieText(c_Painter, mc_STYLE_GUIDE_COLOR_0, c_InnerCircleRect);

            c_Painter.restore();
            QWidget::paintEvent(opc_Event);
         }
         else // Gradation BRIGHT
         {
            f32_PieWidth = 6.0;
            f32_RimWidth = 6.0;
            f32_PieGap = f32_PieWidth / 2.0 + 2.0;
            f32_GesamtPixel = 146.0;

            m_SetAllPieRects(c_InnerCircleRect, c_RimRect, c_PieRect, f32_PieGap, f32_PieWidth, f32_RimWidth,
                             f32_GesamtPixel, 2.0);

            // Damit die Kanten der Kreise geglättet werden
            c_Painter.setRenderHint(QPainter::Antialiasing);

            // Pie--------------------------------------------------------------------------------
            c_Gradient.setCenter(c_InnerCircleRect.center());
            c_Gradient.setAngle(90);
            c_Gradient.setColorAt(1.0, mc_STYLE_GUIDE_COLOR_38);                                                       // Farbe:
                                                                                                                       // 38
            c_Gradient.setColorAt((1.0 - static_cast<float64>(mu16_ValueInPercent) / 100.0), mc_STYLE_GUIDE_COLOR_13); // Farbe:
                                                                                                                       // 13
            m_SetPie(c_Painter, c_Gradient, Qt::FlatCap, c_PieRect, f32_PieWidth, f32_GesamtPixel);

            // Außenrand--------------------------------------------------------------------------
            m_SetPieRim(c_Painter, QBrush(mc_STYLE_GUIDE_COLOR_38), c_RimRect, f32_RimWidth, f32_GesamtPixel);

            // Innenkreis-------------------------------------------------------------------------
            if (mu16_BackgroundTransparnecy == 0)
            {
               m_SetInnerCircle(c_Painter, QBrush(mc_STYLE_GUIDE_COLOR_0), c_InnerCircleRect);
            }
            else
            {
               m_SetInnerCircle(c_Painter, QBrush(QColor(Qt::transparent)), c_InnerCircleRect);
            }

            // Text-------------------------------------------------------------------------------
            m_SetPieText(c_Painter, mc_STYLE_GUIDE_COLOR_6, c_InnerCircleRect);

            c_Painter.restore();
            QWidget::paintEvent(opc_Event);
         }
         break;

      default:
         break;
      }
      */
   //Karsten Ende
}

//-----------------------------------------------------------------------------
void stw_opensyde_gui::C_OgeWiPieChart::m_OnSpinboxValuechanged(const stw_types::sintn osn_Value)
{
   mu16_ValueInPercent = static_cast<uint16>(osn_Value);
   //Karsten Anfang
   /*
   mc_Style = this->mpc_Ui->pc_comboBox->currentIndex();                                           //combobox current
                                                                                                   // item
                                                                                                   //             Style
   mc_Mode = this->mpc_Ui->pc_comboBox_2->currentIndex();                                          //combobox current
                                                                                                   // item
                                                                                                   //            Mode
   mu16_BackgroundRed = static_cast<uint16>(this->mpc_Ui->pc_SpinBox_2->value());                  // Background Color
                                                                                                   // Red Part
   mu16_BackgroundGreen = static_cast<uint16>(this->mpc_Ui->pc_SpinBox_3->value());                // Background Color
                                                                                                   // Green Part
   mu16_BackgroundBlue = static_cast<uint16>(this->mpc_Ui->pc_SpinBox_4->value());                 // Background Color
                                                                                                   // Blue Part
   mu16_BackgroundTransparnecy = static_cast<uint16>(this->mpc_Ui->pc_comboBox_3->currentIndex()); // Pie InnerCircle
                                                                                                   // Transparency
   */
   //Karsten Ende
   this->repaint();
}

// Karsten Anfang
/*
//-----------------------------------------------------------------------------
void stw_opensyde_gui::C_OgeWiPieChart::m_SetPie(QPainter & orc_Painter, QBrush oc_PieBrush,
                                                 Qt::PenCapStyle oc_CapStyle, QRect & orc_PieCircleRect,
                                                 stw_types::float32 of32_PieWidth, stw_types::float32 of32_GesamtPixel)
{
   QPen c_PieArc;

   c_PieArc.setBrush(oc_PieBrush); // Farbe
   // Setzt die Breite ein wenig größer, damit eine Überlappung entsteht, da ansonsten bei Resice Lücken auftreten
   // können
   c_PieArc.setWidth(std::ceil(orc_PieCircleRect.height() * (of32_PieWidth + 1.0) / of32_GesamtPixel));
   c_PieArc.setCapStyle(oc_CapStyle); //Qt::RoundCap sorgt für Überlappungen & default geht nicht bei Gradient
   orc_Painter.setPen(c_PieArc);
   stw_types::sint16 s16_PieStartAngle = 90 * 16; // Um Oben in der Mitte zu beginnen
   stw_types::sint16 s16_PieSpanAngle =           // -1: mit dem Uhrzeigersinn, 360*16: Komplette umdrehung
                                        static_cast<sint16>(-1.0 * (360.0 / 100.0) *
                                                            static_cast<float64>(mu16_ValueInPercent)  * 16.0);

   orc_Painter.drawArc(orc_PieCircleRect, s16_PieStartAngle, s16_PieSpanAngle); // Zeichnet den Bogen im Rechteck von
                                                                                // dem
   // Punkten Start mit der Länge Span innerhalb des Rechtecks
}

//-----------------------------------------------------------------------------
void stw_opensyde_gui::C_OgeWiPieChart::m_SetPieDot(QPainter & orc_Painter, QColor oc_PieDotColor, QRect oc_PieRect,
                                                    stw_types::float32 of32_DotSize,
                                                    stw_types::float32 of32_GesamtPixel)
{
   stw_types::sint16 s16_DotPositionX; // Variable for X-Postition
   stw_types::sint16 s16_DotPositionY; // Variable for Y-Position
   stw_types::sint16 s16_DotSize;      // Durchmesser der Kugel
   orc_Painter.setPen(Qt::NoPen);
   orc_Painter.setBrush(QBrush(QColor(oc_PieDotColor)));                 // Color of the Dot
   s16_DotPositionX = oc_PieRect.left();                                 // Initial value
   s16_DotPositionY = oc_PieRect.top();                                  // Initial value
   s16_DotSize = (oc_PieRect.width()) * of32_DotSize / of32_GesamtPixel; // Durchmesser des Punktes in Abhängigkeit der
                                                                         // Pie Größe

   // Kugel sitzt auf Außenrand des Keises
   s16_DotPositionX = ((oc_PieRect.right() + oc_PieRect.left()) / 2.0 - s16_DotSize / 2.0) +
                      cos(static_cast<float64>(mu16_ValueInPercent) / 100.0 * 2.0 * M_PI - M_PI / 2.0) *
                      oc_PieRect.width() / 2.0;
   s16_DotPositionY = ((oc_PieRect.bottom() + oc_PieRect.top()) / 2.0 - s16_DotSize / 2.0) -
                      sin(static_cast<float64>(mu16_ValueInPercent) / 100.0 * 2.0 * M_PI + M_PI / 2.0) *
                      oc_PieRect.height() / 2.0;

   orc_Painter.drawEllipse(s16_DotPositionX, s16_DotPositionY, s16_DotSize, s16_DotSize);
}

//-----------------------------------------------------------------------------
void stw_opensyde_gui::C_OgeWiPieChart::m_SetPieRim(QPainter & orc_Painter, QBrush oc_RimBrush, QRect oc_RimCircleRect,
                                                    stw_types::float32 of32_RimSize,
                                                    stw_types::float32 of32_GesamtPixel,
                                                    stw_types::float32 of32_ReductionFactor)
{
   QPen mc_RandArc;

   mc_RandArc.setBrush(oc_RimBrush); // Farbe
   // ReductionFactor to close or open gaps between the circles
   mc_RandArc.setWidth(std::ceil(this->rect().height() * (of32_RimSize - of32_ReductionFactor) / of32_GesamtPixel));
   mc_RandArc.setCapStyle(Qt::FlatCap);
   orc_Painter.setPen(mc_RandArc);
   // Arc with complete turn
   orc_Painter.drawArc(oc_RimCircleRect, static_cast<sint16>(90 * 16), static_cast<sint16>(-360.0  * 16.0));
}

//-----------------------------------------------------------------------------
void stw_opensyde_gui::C_OgeWiPieChart::m_SetInnerCircle(QPainter & orc_Painter, QBrush oc_InnerCircleBrush,
                                                         QRect oc_InnerCircleRect)
{
   // Painting the inner Circle
   orc_Painter.setPen(Qt::NoPen);
   orc_Painter.setBrush(oc_InnerCircleBrush);
   orc_Painter.drawEllipse(oc_InnerCircleRect);
}

//-----------------------------------------------------------------------------
void stw_opensyde_gui::C_OgeWiPieChart::m_SetPieText(QPainter & orc_Painter, QColor oc_TextColor,
                                                     QRect oc_InnerCircleRect)
{
   QPen c_TextPen;
   QFont c_TextFont;

   stw_scl::C_SCLString m_String;
   c_TextPen.setColor(oc_TextColor);
   c_TextFont.setFamily("Segoe UI");
   c_TextFont.setPointSize(44);
   orc_Painter.setPen(c_TextPen);
   orc_Painter.setFont(c_TextFont);
   m_String = stw_scl::C_SCLString::IntToStr(mu16_ValueInPercent);
   m_String += "%";
   orc_Painter.drawText(oc_InnerCircleRect, static_cast<sintn>(Qt::AlignCenter), m_String.c_str());
}

//-----------------------------------------------------------------------------
void stw_opensyde_gui::C_OgeWiPieChart::m_SetAllPieRects(QRect & orc_InnerCircleRect, QRect & orc_RimCircleRect,
                                                         QRect & orc_PieCircleRect, stw_types::float32 of32_PieGap,
                                                         stw_types::float32 of32_PieSize,
                                                         stw_types::float32 of32_RimSize,
                                                         stw_types::float32 of32_GesamtPixel,
                                                         stw_types::float32 of32_AdditionalFactor)
{
   orc_InnerCircleRect.setRect(
      (this->rect().left() + this->rect().width() *
       (of32_PieGap + of32_PieSize / 2.0 + of32_RimSize) / of32_GesamtPixel) * 0.5 - of32_AdditionalFactor,
      this->rect().top() + this->rect().height() *
      (of32_PieGap + of32_PieSize / 2.0 + of32_RimSize) / of32_GesamtPixel - of32_AdditionalFactor,
      this->rect().width() *
      (of32_GesamtPixel - 2.0 * (of32_PieGap + of32_PieSize / 2.0 + of32_RimSize)) / of32_GesamtPixel * 0.5 + 2.0 * of32_AdditionalFactor,
      this->rect().height() *
      (of32_GesamtPixel - 2.0 * (of32_PieGap + of32_PieSize / 2.0 + of32_RimSize)) / of32_GesamtPixel + 2.0 *
      of32_AdditionalFactor);
   orc_RimCircleRect.setRect((this->rect().left() + this->rect().width() *
                              (of32_PieGap + of32_PieSize / 2.0 + of32_RimSize / 2.0) / of32_GesamtPixel) * 0.5,
                             this->rect().top() + this->rect().height() *
                             (of32_PieGap + of32_PieSize / 2.0 + of32_RimSize / 2.0) / of32_GesamtPixel,
                             this->rect().width() *
                             (of32_GesamtPixel - 2.0 * (of32_PieGap + of32_PieSize / 2.0 + of32_RimSize / 2.0)) / of32_GesamtPixel * 0.5,
                             this->rect().height() *
                             (of32_GesamtPixel - 2.0 * (of32_PieGap + of32_PieSize / 2.0 + of32_RimSize / 2.0)) /
                             of32_GesamtPixel);
   orc_PieCircleRect.setRect((this->rect().left() + this->rect().width() * of32_PieGap / of32_GesamtPixel) * 0.5,
                             this->rect().top() + this->rect().height() * of32_PieGap / of32_GesamtPixel,
                             this->rect().width() * (of32_GesamtPixel - 2.0 * of32_PieGap) / of32_GesamtPixel * 0.5,
                             this->rect().height() * (of32_GesamtPixel - 2.0 * of32_PieGap) / of32_GesamtPixel);
}
*/
// Karsten Ende
