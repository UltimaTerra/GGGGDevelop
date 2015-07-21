/**

GDevelop - Primitive Drawing Extension
Copyright (c) 2008-2015 Florian Rival (Florian.Rival@gmail.com)
This project is released under the MIT License.
*/

#if defined(GD_IDE_ONLY) && !defined(GD_NO_WX_GUI)
#include <wx/wx.h> //Must be placed first, otherwise we get errors relative to "cannot convert 'const TCHAR*'..." in wx/msw/winundef.h
#endif
#include <SFML/Graphics.hpp>
#include "GDCore/Tools/Localization.h"
#include "GDCpp/Object.h"
#include "GDCpp/RuntimeScene.h"
#include "GDCpp/Project.h"
#include "GDCpp/ImageManager.h"
#include "GDCpp/Polygon2d.h"
#include "GDCpp/Serialization/SerializerElement.h"
#include "GDCpp/FontManager.h"
#include "GDCpp/Position.h"
#include "GDCpp/CommonTools.h"
#include "ShapePainterObject.h"

#if defined(GD_IDE_ONLY)
#include "GDCpp/CommonTools.h"
#include "GDCore/IDE/Dialogs/MainFrameWrapper.h"
#include "ShapePainterObjectEditor.h"
#endif

#if defined(GD_IDE_ONLY)
sf::Texture ShapePainterObject::edittimeIconImage;
sf::Sprite ShapePainterObject::edittimeIcon;
#endif

using namespace std;

ShapePainterObjectBase::ShapePainterObjectBase() :
    fillColorR( 255 ),
    fillColorG( 255 ),
    fillColorB( 255 ),
    fillOpacity( 255 ),
    outlineSize(1),
    outlineColorR(0),
    outlineColorG(0),
    outlineColorB(0),
    outlineOpacity(255),
    absoluteCoordinates(false)
{
}

ShapePainterObject::ShapePainterObject(gd::String name_) :
    gd::Object(name_)
{
}

RuntimeShapePainterObject::RuntimeShapePainterObject(RuntimeScene & scene, const gd::Object & object) :
    RuntimeObject(scene, object)
{
    const ShapePainterObject & drawerObject = static_cast<const ShapePainterObject&>(object);
    ShapePainterObjectBase::operator=(drawerObject);
}

void ShapePainterObjectBase::UnserializeFrom(const gd::SerializerElement & element)
{
    fillOpacity = element.GetChild("fillOpacity", 0, "FillOpacity").GetValue().GetInt();
    outlineSize = element.GetChild("outlineSize", 0, "OutlineSize").GetValue().GetInt();
    outlineOpacity = element.GetChild("outlineOpacity", 0, "OutlineOpacity").GetValue().GetInt();

    fillColorR = element.GetChild("fillColor", 0, "FillColor").GetIntAttribute("r");
    fillColorG = element.GetChild("fillColor", 0, "FillColor").GetIntAttribute("g");
    fillColorB = element.GetChild("fillColor", 0, "FillColor").GetIntAttribute("b");

    outlineColorR = element.GetChild("outlineColor", 0, "OutlineColor").GetIntAttribute("r");
    outlineColorG = element.GetChild("outlineColor", 0, "OutlineColor").GetIntAttribute("g");
    outlineColorB = element.GetChild("outlineColor", 0, "OutlineColor").GetIntAttribute("b");

    absoluteCoordinates = element.GetChild("absoluteCoordinates", 0, "AbsoluteCoordinates").GetValue().GetBool();
}

void ShapePainterObject::DoUnserializeFrom(gd::Project & project, const gd::SerializerElement & element)
{
    ShapePainterObjectBase::UnserializeFrom(element);
}

#if defined(GD_IDE_ONLY)
void ShapePainterObjectBase::SerializeTo(gd::SerializerElement & element) const
{
    element.AddChild("fillOpacity").SetValue(fillOpacity);
    element.AddChild("outlineSize").SetValue(outlineSize);
    element.AddChild("outlineOpacity").SetValue(outlineOpacity);
    element.AddChild("fillColor")
        .SetAttribute("r", (int)fillColorR)
        .SetAttribute("g", (int)fillColorG)
        .SetAttribute("b", (int)fillColorB);
    element.AddChild("outlineColor")
        .SetAttribute("r", (int)outlineColorR)
        .SetAttribute("g", (int)outlineColorG)
        .SetAttribute("b", (int)outlineColorB);
    element.AddChild("absoluteCoordinates").SetValue(absoluteCoordinates);
}

void ShapePainterObject::DoSerializeTo(gd::SerializerElement & element) const
{
    ShapePainterObjectBase::SerializeTo(element);
}
#endif

/**
 * Render object at runtime
 */
bool RuntimeShapePainterObject::Draw( sf::RenderTarget& renderTarget )
{
    //Don't draw anything if hidden
    if ( hidden )
    {
        shapesToDraw.clear();
        return true;
    }

    for (unsigned int i = 0;i<shapesToDraw.size();++i)
    {
    	renderTarget.draw(shapesToDraw[i].rectangleShape);
    	renderTarget.draw(shapesToDraw[i].circleShape);
    }

    shapesToDraw.clear();

    return true;
}

#if defined(GD_IDE_ONLY)
/**
 * Render object at edittime
 */
void ShapePainterObject::DrawInitialInstance(gd::InitialInstance & instance, sf::RenderTarget & renderTarget, gd::Project & project, gd::Layout & layout)
{
    edittimeIcon.setPosition(instance.GetX(), instance.GetY());
    renderTarget.draw(edittimeIcon);
}

void ShapePainterObject::LoadEdittimeIcon()
{
    edittimeIconImage.loadFromFile("CppPlatform/Extensions/primitivedrawingicon.png");
    edittimeIcon.setTexture(edittimeIconImage);
}

bool ShapePainterObject::GenerateThumbnail(const gd::Project & project, wxBitmap & thumbnail) const
{
#if !defined(GD_NO_WX_GUI)
    thumbnail = wxBitmap("CppPlatform/Extensions/primitivedrawingicon24.png", wxBITMAP_TYPE_ANY);
#endif

    return true;
}

void ShapePainterObject::EditObject( wxWindow* parent, gd::Project & game, gd::MainFrameWrapper & mainFrameWrapper )
{
#if !defined(GD_NO_WX_GUI)
    ShapePainterObjectEditor dialog(parent, game, *this);
    dialog.ShowModal();
#endif
}

void RuntimeShapePainterObject::GetPropertyForDebugger(unsigned int propertyNb, gd::String & name, gd::String & value) const
{
    if      ( propertyNb == 0 ) {name = _("Fill color");    value = gd::String::From(GetFillColorR())+";"+gd::String::From(GetFillColorG())+";"+gd::String::From(GetFillColorB());}
    else if ( propertyNb == 1 ) {name = _("Fill opacity");    value = gd::String::From(GetFillOpacity());}
    else if ( propertyNb == 2 ) {name = _("Outline size");         value = gd::String::From(GetOutlineSize());}
    else if ( propertyNb == 3 ) {name = _("Outline color");        value = gd::String::From(GetOutlineColorR())+";"+gd::String::From(GetOutlineColorG())+";"+gd::String::From(GetOutlineColorB());}
    else if ( propertyNb == 4 ) {name = _("Outline opacity");        value = gd::String::From(GetOutlineOpacity());}
}

bool RuntimeShapePainterObject::ChangeProperty(unsigned int propertyNb, gd::String newValue)
{
    if ( propertyNb == 0 )
    {
        std::vector < gd::String > colors = newValue.Split(U';');
        if ( colors.size() < 3 ) return false; //Color is not valid

        SetFillColor(colors[0].ToInt(), colors[1].ToInt(), colors[2].ToInt());
    }
    else if ( propertyNb == 1 ) { SetFillOpacity(newValue.ToFloat()); }
    else if ( propertyNb == 2 ) { SetOutlineSize(newValue.ToInt()); }
    else if ( propertyNb == 3 )
    {
        std::vector < gd::String > colors = newValue.Split(U';');
        if ( colors.size() < 3 ) return false; //Color is not valid

        SetOutlineColor(colors[0].ToInt(), colors[1].ToInt(), colors[2].ToInt());
    }
    else if ( propertyNb == 4 ) { SetOutlineOpacity(newValue.ToFloat()); }

    return true;
}

unsigned int RuntimeShapePainterObject::GetNumberOfProperties() const
{
    return 5;
}
#endif

/**
 * Change the color filter of the sprite object
 */
void ShapePainterObjectBase::SetFillColor( unsigned int r, unsigned int g, unsigned int b )
{
    fillColorR = r;
    fillColorG = g;
    fillColorB = b;
}

void ShapePainterObjectBase::SetFillOpacity(float val)
{
    if ( val > 255 )
        val = 255;
    else if ( val < 0 )
        val = 0;

    fillOpacity = val;
}

/**
 * Change the color filter of the sprite object
 */
void ShapePainterObjectBase::SetOutlineColor( unsigned int r, unsigned int g, unsigned int b )
{
    outlineColorR = r;
    outlineColorG = g;
    outlineColorB = b;
}

void ShapePainterObjectBase::SetOutlineOpacity(float val)
{
    if ( val > 255 )
        val = 255;
    else if ( val < 0 )
        val = 0;

    outlineOpacity = val;
}

/**
 * Change the fill color
 */
void ShapePainterObjectBase::SetFillColor( const gd::String & color )
{
    std::vector < gd::String > colors = color.Split(U';');
    if ( colors.size() < 3 ) return;

    fillColorR = colors[0].ToInt();
    fillColorG = colors[1].ToInt();
    fillColorB = colors[2].ToInt();
}

/**
 * Change the color of the outline
 */
void ShapePainterObjectBase::SetOutlineColor( const gd::String & color )
{
    std::vector < gd::String > colors = color.Split(U';');
    if ( colors.size() < 3 ) return;

    outlineColorR = colors[0].ToInt();
    outlineColorG = colors[1].ToInt();
    outlineColorB = colors[2].ToInt();
}

void RuntimeShapePainterObject::DrawRectangle( float x, float y, float x2, float y2 )
{
    float Xgap = AreCoordinatesAbsolute() ? 0 : GetX();
    float Ygap = AreCoordinatesAbsolute() ? 0 : GetY();

    DrawingCommand command(sf::RectangleShape(sf::Vector2f(x2-x, y2-y)));
    command.rectangleShape.setPosition(x+Xgap, y+Ygap);
    command.rectangleShape.setFillColor(sf::Color(GetFillColorR(), GetFillColorG(), GetFillColorB(), GetFillOpacity()));
    command.rectangleShape.setOutlineThickness(GetOutlineSize());
    command.rectangleShape.setOutlineColor(sf::Color(GetOutlineColorR(), GetOutlineColorG(), GetOutlineColorB(), GetOutlineOpacity()));

    shapesToDraw.push_back(command);
}

void RuntimeShapePainterObject::DrawLine( float x, float y, float x2, float y2, float thickness )
{
    float Xgap = AreCoordinatesAbsolute() ? 0 : GetX();
    float Ygap = AreCoordinatesAbsolute() ? 0 : GetY();

    float length = sqrt((x-x2)*(x-x2)+(y-y2)*(y-y2));
    DrawingCommand command(sf::RectangleShape(sf::Vector2f(length, thickness)));
    command.rectangleShape.setPosition((x+x2)/2+Xgap, (y+y2)/2+Ygap);
    command.rectangleShape.setOrigin(length/2, thickness/2);
    command.rectangleShape.setRotation(atan2(y2-y,x2-x)*180/3.14159);
    command.rectangleShape.setFillColor(sf::Color(GetFillColorR(), GetFillColorG(), GetFillColorB(), GetFillOpacity()));
    command.rectangleShape.setOutlineThickness(GetOutlineSize());
    command.rectangleShape.setOutlineColor(sf::Color(GetOutlineColorR(), GetOutlineColorG(), GetOutlineColorB(), GetOutlineOpacity()));

    shapesToDraw.push_back(command);
}

void RuntimeShapePainterObject::DrawCircle( float x, float y, float radius )
{
    float Xgap = AreCoordinatesAbsolute() ? 0 : GetX();
    float Ygap = AreCoordinatesAbsolute() ? 0 : GetY();

    sf::CircleShape circle(radius);
    DrawingCommand command(circle);
    command.circleShape.setPosition(x+Xgap, y+Ygap);
    command.circleShape.setOrigin(radius, radius);
    command.circleShape.setFillColor(sf::Color(GetFillColorR(), GetFillColorG(), GetFillColorB(), GetFillOpacity()));
    command.circleShape.setOutlineThickness(GetOutlineSize());
    command.circleShape.setOutlineColor(sf::Color(GetOutlineColorR(), GetOutlineColorG(), GetOutlineColorB(), GetOutlineOpacity()));

    shapesToDraw.push_back(command);
}

RuntimeObject * CreateRuntimeShapePainterObject(RuntimeScene & scene, const gd::Object & object)
{
    return new RuntimeShapePainterObject(scene, object);
}

gd::Object * CreateShapePainterObject(gd::String name)
{
    return new ShapePainterObject(name);
}
