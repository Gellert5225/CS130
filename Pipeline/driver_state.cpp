#include "driver_state.h"
#include <cstring>

driver_state::driver_state()
{
}

driver_state::~driver_state()
{
    delete [] image_color;
    delete [] image_depth;
}

// This function should allocate and initialize the arrays that store color and
// depth.  This is not done during the constructor since the width and height
// are not known when this class is constructed.
void initialize_render(driver_state& state, int width, int height)
{
    state.image_width = width;
    state.image_height = height;
    state.image_color = 0;
    state.image_depth = 0;
    
    state.image_color = new pixel[width * height];
    for (int i = 0; i < width * height; i++) {
        state.image_color[i] = make_pixel(0, 0, 0);
    }
}

// This function will be called to render the data that has been stored in this class.
// Valid values of type are:
//   render_type::triangle - Each group of three vertices corresponds to a triangle.
//   render_type::indexed -  Each group of three indices in index_data corresponds
//                           to a triangle.  These numbers are indices into vertex_data.
//   render_type::fan -      The vertices are to be interpreted as a triangle fan.
//   render_type::strip -    The vertices are to be interpreted as a triangle strip.
void render(driver_state& state, render_type type)
{
    switch (type){
        case render_type::triangle: {
            // printf("number of triangles: %i\n", state.num_vertices / 3);
            // printf("vertex data:\n");
            // for (int i = 0; i < state.num_vertices; i++) {
            //     for (int j = 0; j < state.floats_per_vertex; j++){
            //         printf("%f ", state.vertex_data[j + i * state.floats_per_vertex]);
            //     }
            //     printf("\n");
            // }
            
            for (int k = 0; k < state.num_vertices / 3; k++) { // for each triangle
                const data_geometry* in[3]; 
                for (int j = 0; j < 3; j++) { // for each vertex
                    data_geometry *d_g = new data_geometry();
                    data_vertex d_v;
                    d_v.data = new float[MAX_FLOATS_PER_VERTEX];
                    d_g->data = new float[MAX_FLOATS_PER_VERTEX];
                    for (int i = 0; i < state.floats_per_vertex; i++) { // for each float in vertex
                        d_g->data[i] = state.vertex_data[i + j * state.floats_per_vertex + k * state.floats_per_vertex * 3];
                        d_v.data[i] = state.vertex_data[i + j * state.floats_per_vertex + k * state.floats_per_vertex * 3];
                    }
                    state.vertex_shader(d_v, *d_g, state.uniform_data);
                    in[j] = d_g;
                } 
                
                rasterize_triangle(state, in);
            }
              
            break;
        }
        default:
            break;
    }
}
// This function clips a triangle (defined by the three vertices in the "in" array).
// It will be called recursively, once for each clipping face (face=0, 1, ..., 5) to
// clip against each of the clipping faces in turn.  When face=6, clip_triangle should
// simply pass the call on to rasterize_triangle.
void clip_triangle(driver_state& state, const data_geometry* in[3], int face)
{
    if(face==6)
    {
        rasterize_triangle(state, in);
        return;
    }
    std::cout<<"TODO: implement clipping. (The current code passes the triangle through without clipping them.)"<<std::endl;
    clip_triangle(state,in,face+1);
}

// Rasterize the triangle defined by the three vertices in the "in" array.  This
// function is responsible for rasterization, interpolation of data to
// fragments, calling the fragment shader, and z-buffering.
void rasterize_triangle(driver_state& state, const data_geometry* in[3])
{
    vec4 points[3];
    // printf("float per vertex: %i\n", state.floats_per_vertex);
    
    for (int j = 0; j < 3; j++) {
        data_geometry dg;
        data_vertex dv; 
        dv.data = new float[MAX_FLOATS_PER_VERTEX];
        for (int i = 0; i < 4; i++) {
            dv.data[i] = in[j]->gl_Position[i];
            //printf("%f ", in[j]->data[i]);
        }
        //printf("\n");
        points[j] = vec4(dv.data[0]/dv.data[3], dv.data[1]/dv.data[3], dv.data[2]/dv.data[3], dv.data[3]);
    }
    
    
    int width = state.image_width;
    int height = state.image_height;

    // from NDC to pixel space
    float a_x = 0.5 * (1 + points[0][0]) * width - 0.5;
    float a_y = 0.5 * (1 + points[0][1]) * height - 0.5;
    float b_x = 0.5 * (1 + points[1][0]) * width - 0.5;
    float b_y = 0.5 * (1 + points[1][1]) * height - 0.5;
    float c_x = 0.5 * (1 + points[2][0]) * width - 0.5;
    float c_y = 0.5 * (1 + points[2][1]) * height - 0.5;    

    float p_x, p_y;
    float alpha, beta, gamma;
    vec2 v0 = {b_x - a_x, b_y - a_y};
    vec2 v1 = {c_x - a_x, c_y - a_y};
    vec2 v2;

    float ABC = dot(v0, v0) * dot(v1, v1) - dot(v0, v1) * dot(v0, v1);

    for (int p = 0; p < width * height; p++) {
        // map index to pixel coord
        p_x = p % width;
        p_y = p / width;

        // calculate barycentric coord
        v2 = {p_x - a_x, p_y - a_y};
        beta = (dot(v1, v1) * dot(v2, v0) - dot(v0, v1) * dot(v2, v1)) / ABC;
        gamma = (dot(v0, v0) * dot(v2, v1) - dot(v0, v1) * dot(v2, v0)) / ABC;
        alpha = 1.0 - beta - gamma;

        // color pixel
        if (beta >= 0 && gamma >= 0 && alpha >= 0) {

            // interpolation
            data_fragment df;
            df.data = new float[MAX_FLOATS_PER_VERTEX];
            data_output out;
            for (int i = 0; i < state.floats_per_vertex; i++) {
                switch (state.interp_rules[i])
                {
                case interp_type::flat:
                    df.data[i] = in[0]->data[i];
                    break;
                case interp_type::smooth:
                    break;
                case interp_type::noperspective: 
                    df.data[i] = alpha * in[0]->data[i] + beta * in[1]->data[i] + gamma * in[2]->data[i];
                    break;
                default:
                    break;
                }
            
            }
            state.fragment_shader(df, out, state.uniform_data);
            state.image_color[p] = make_pixel(out.output_color[0] * 255.0, 
            out.output_color[1] * 255.0, 
            out.output_color[2] * 255.0);
        }
    }
}