subroutine (GM_TechniqueEntrance)
void GM_Model2D()
{
    if (GM_AmbientTextureAttribute.Enabled == 1)
    {
        _frag_color = texture(GM_AmbientTextureAttribute.Texture, _uv * vec2(GM_AmbientTextureAttribute.ScaleX, GM_AmbientTextureAttribute.ScaleY));
        _frag_color *= _color;
    }
    else
    {
        _frag_color = vec4(0, 0, 0, 0);
    }
}
