#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform sampler2D texture_diffuse0;
uniform vec3 light_pos;
uniform vec3 view_pos;
uniform vec4 light_color;

void main()
{           
    vec3 color = texture(texture_diffuse0, fs_in.TexCoords).rgb;
    // ambient
    vec3 ambient = 0.05 * color;
    // diffuse
    vec3 light_dir = normalize(light_pos - fs_in.FragPos);
    vec3 normal = normalize(fs_in.Normal);
    float diff = max(dot(light_dir, normal), 0.0);
    vec3 diffuse = diff * color * light_color.rgb;
    // specular
    vec3 view_dir = normalize(view_pos - fs_in.FragPos);
    float spec = 0.0;

    vec3 halfway_dir = normalize(light_dir + view_dir);  
    spec = pow(max(dot(normal, halfway_dir), 0.0), 32.0);

    vec3 specular = vec3(0.3) * spec; 
    FragColor = vec4(ambient + diffuse + specular, 1.0);
}