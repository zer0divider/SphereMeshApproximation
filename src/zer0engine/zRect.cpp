#include "zRect.h"

using namespace zer0;

bool Rect::intersect(const Rect & a, const Rect & b, Rect * intersection_rect)
{
    if(a.x+a.w > b.x-b.w && a.x-a.w < b.x+b.w &&
        a.y+a.h > b.y-b.h && a.y-a.h < b.y+b.h){
        if(intersection_rect != NULL){
            float left = max(a.x-a.w, b.x-b.w);
            float right = min(a.x+a.w, b.x+b.w);
            float down = max(a.y-a.h, b.y-b.h);
            float up = min(a.y+a.h, b.y+b.h);
            intersection_rect->set((right+left)/2.f, (up+down)/2.f, (right-left)/2.f, (up-down)/2.f);
        }
        return true;
    }
    else{
        return false;
    }
}

bool Rect::contains(const Rect & r, float epsilon)const
{
    return r.x+r.w <= x+w+epsilon && r.x-r.w >= x-w-epsilon &&
         r.y+r.h <= y+h+epsilon && r.y-r.h >= y-h-epsilon;
}


bool Rect::checkPoint(const Vector2D & point)const
{
    return point.x <= x+w && point.x >= x-w && point.y <= y+h && point.y >= y-h;
}