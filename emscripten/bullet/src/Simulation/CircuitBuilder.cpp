

#include "CircuitBuilder.hpp"

#include "Utility/BSpline.hpp"
#include "Utility/TraceLogger.hpp"

#include <algorithm>
#include <fstream>
#include <sstream>

#include <cmath>


namespace /* anonymous */
{

	inline void	ltrim(std::string &s)
	{
	    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
	        return !std::isspace(ch);
	    }));
	}

	inline void	rtrim(std::string &s)
	{
	    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
	        return !std::isspace(ch);
	    }).base(), s.end());
	}

	inline void	trim(std::string &s)
	{
	    ltrim(s);
	    rtrim(s);
	}

	void	calculate_normal(float* in_v0, float* in_v1, float* in_v2, float* out_normal)
	{
		float px = in_v1[0] - in_v0[0];
		float py = in_v1[1] - in_v0[1];
		float pz = in_v1[2] - in_v0[2];

		float qx = in_v2[0] - in_v0[0];
		float qy = in_v2[1] - in_v0[1];
		float qz = in_v2[2] - in_v0[2];

		out_normal[0] = (py * qz) - (pz * qy);
		out_normal[1] = (pz * qx) - (px * qz);
		out_normal[2] = (px * qy) - (py * qx);

		float tmp_len = sqrtf(
			out_normal[0]*out_normal[0] +
			out_normal[1]*out_normal[1] +
			out_normal[2]*out_normal[2]
		);

		out_normal[0] /= tmp_len;
		out_normal[1] /= tmp_len;
		out_normal[2] /= tmp_len;
	}

};

//
//

bool	CircuitBuilder::load(const std::string& filename)
{
	//
	//
	// open/read file

	std::ifstream infile(filename);

	if (!infile.is_open())
		return false;

	//
	//
	// parse file and extract skeleton

	std::vector<float>	arr_left, arr_right;

	std::string str_line;
	while (std::getline(infile, str_line))
	{
		trim(str_line);

		if (str_line.empty() ||
			str_line.find("PUSH") != 0)
			continue;

		std::istringstream isstr(str_line);

		std::string str_type;
		float x1, y1, z1,  x2, y2, z2;
		if (!(isstr >> str_type >> x1 >> y1 >> z1 >> x2 >> y2 >> z2)) // TODO: <= what if it get a NaN?
		{
			D_MYLOG("failure");
			return false;
		}

		arr_left.push_back(x1);
		arr_left.push_back(y1);
		arr_left.push_back(z1);

		arr_right.push_back(x2);
		arr_right.push_back(y2);
		arr_right.push_back(z2);
	}

	//
	//
	// concatenate skeleton

	for (int index = 0; index < arr_left.size(); index += 3)
	{
		float x1 = arr_left[index+0];
		float y1 = arr_left[index+1];
		float z1 = arr_left[index+2];

		float x2 = arr_right[index+0];
		float y2 = arr_right[index+1];
		float z2 = arr_right[index+2];

	    if (index > 0)
	    {
	        // concatenate the point

	        float prev_x1 = m_points_left[(index-3)+0];
	        float prev_y1 = m_points_left[(index-3)+1];
	        float prev_z1 = m_points_left[(index-3)+2];

	        float prev_x2 = m_points_right[(index-3)+0];
	        float prev_y2 = m_points_right[(index-3)+1];
	        float prev_z2 = m_points_right[(index-3)+2];

	        x1 += prev_x1;
	        y1 += prev_y1;
	        z1 += prev_z1;

	        x2 += prev_x2;
	        y2 += prev_y2;
	        z2 += prev_z2;
	    }

	    m_points_left.push_back(x1);
	    m_points_left.push_back(y1);
	    m_points_left.push_back(z1);

	    m_points_right.push_back(x2);
	    m_points_right.push_back(y2);
	    m_points_right.push_back(z2);
	}

    return true;
}

//
//

bool	CircuitBuilder::generate_skeleton(t_callback skeleton)
{
	if (!skeleton ||
		m_points_left.empty() ||
		m_points_right.empty())
	{
		return false;
	}

	std::vector<float>	buffer;
	buffer.reserve(400);

	std::vector<int> 	indices;

#define D_PUSH_VERTEX(container, v1,v2,v3)	\
	(container).push_back(v1);	\
	(container).push_back(v2);	\
	(container).push_back(v3);


	int index2 = 0;

	for (int index = 0; index < m_points_left.size(); index += 3)
	{
		D_PUSH_VERTEX(buffer, m_points_left[index+0],m_points_left[index+1],m_points_left[index+2]);
		D_PUSH_VERTEX(buffer, m_points_right[index+0],m_points_right[index+1],m_points_right[index+2]);

		D_PUSH_VERTEX(buffer, m_points_left[index+0],m_points_left[index+1],0);
		D_PUSH_VERTEX(buffer, m_points_right[index+0],m_points_right[index+1],0);

		int step = index2 * 4;

		indices.push_back(step+0); // upper 0 left-right
		indices.push_back(step+1);

		indices.push_back(step+2); // lower 0 left-right
		indices.push_back(step+3);

		indices.push_back(step+0); // lower-upper 0 left
		indices.push_back(step+2);

		indices.push_back(step+1); // lower-upper 0 right
		indices.push_back(step+3);

		if (index2 > 0)
		{
			indices.push_back(step+0); // upper 0-1 left
			indices.push_back(step+0-4);

			indices.push_back(step+1); // upper 0-1 right
			indices.push_back(step+1-4);

			indices.push_back(step+2); // lower 0-1 left
			indices.push_back(step+2-4);

			indices.push_back(step+3); // lower 0-1 right
			indices.push_back(step+3-4);
		}

		++index2;
	}

#undef D_PUSH_VERTEX

	skeleton(buffer, indices);

	return true;
}

bool	CircuitBuilder::generate(t_callback ground, t_callback wall)
{
    //
    //
    // smooth the circuit

    std::vector<float> arr_spline_left;
    std::vector<float> arr_spline_right;

    BSpline	spline1(m_points_left, 3, 3);
    BSpline	spline2(m_points_right, 3, 3);

	for (float t = 0.0f; t <= 1.0f; t += 0.001f) // small steps
	{
		float	arr_res1[3];
		float	arr_res2[3];

		spline1.calcAt(t, arr_res1);
		spline2.calcAt(t, arr_res2);

		if (!arr_spline_left.empty())
		{
			unsigned int index = arr_spline_left.size() - 3;

			//

			float diff_x1 = arr_res1[0] - arr_spline_left[index + 0];
			float diff_y1 = arr_res1[1] - arr_spline_left[index + 1];
			float diff_z1 = arr_res1[2] - arr_spline_left[index + 2];

			float diff_x2 = arr_res2[0] - arr_spline_right[index + 0];
			float diff_z2 = arr_res2[1] - arr_spline_right[index + 1];
			float diff_y2 = arr_res2[2] - arr_spline_right[index + 2];

            float len1 = sqrtf(diff_x1*diff_x1 + diff_y1*diff_y1 + diff_z1*diff_z1);
            float len2 = sqrtf(diff_x2*diff_x2 + diff_y2*diff_y2 + diff_z2*diff_z2);

            // do not add steps that are too close from each other
            if (len1 < 2 && len2 < 2)
                continue;
		}

		arr_spline_left.push_back(arr_res1[0]);
		arr_spline_left.push_back(arr_res1[1]);
		arr_spline_left.push_back(arr_res1[2]);

		arr_spline_right.push_back(arr_res2[0]);
		arr_spline_right.push_back(arr_res2[1]);
		arr_spline_right.push_back(arr_res2[2]);
	}

    //
    //
    // generate circuit

    int geometry_step = 6;

	float pPreviousNormal[3];

    for (int index = 3; index < arr_spline_left.size(); index += 3 * geometry_step)
    {

		std::vector<float>	vertices_ground;
		std::vector<float>	vertices_walls1;
		std::vector<float>	vertices_walls2;

		std::vector<int> 	indices;

		//
		//
		// prepare the indices

		indices.push_back(0);
		indices.push_back(1);
		indices.push_back(2);
		indices.push_back(0);
		indices.push_back(3);
		indices.push_back(2);

        //
        //
        // prepare the vertices

    	{ // push ground

#define D_PUSH_VERTEX(in_container, in_x, in_y, in_z)	\
	(in_container).push_back( (in_x) );	\
	(in_container).push_back( (in_y) );	\
	(in_container).push_back( (in_z) );

			int index2 = 0;

	        for (int index5 = index;
	        	index5 < arr_spline_left.size() &&
	        	index5 < index + geometry_step * 3;
	        	index5 += 3)
	        {

				if (index2 > 0)
				{
					int step = index2 * 4;

					indices.push_back(indices[0]+step);
					indices.push_back(indices[1]+step);
					indices.push_back(indices[2]+step);

					indices.push_back(indices[3]+step);
					indices.push_back(indices[4]+step);
					indices.push_back(indices[5]+step);
				}

				++index2;


				float x1 = arr_spline_left[index5+0];
				float y1 = arr_spline_left[index5+1];
				float z1 = arr_spline_left[index5+2];

				float x2 = arr_spline_right[index5+0];
				float y2 = arr_spline_right[index5+1];
				float z2 = arr_spline_right[index5+2];

				float prev_x1 = arr_spline_left[(index5-3)+0];
				float prev_y1 = arr_spline_left[(index5-3)+1];
				float prev_z1 = arr_spline_left[(index5-3)+2];

				float prev_x2 = arr_spline_right[(index5-3)+0];
				float prev_y2 = arr_spline_right[(index5-3)+1];
				float prev_z2 = arr_spline_right[(index5-3)+2];


				D_PUSH_VERTEX(vertices_ground,  x1, y1, z1);
				D_PUSH_VERTEX(vertices_ground,  x2, y2, z2);
				D_PUSH_VERTEX(vertices_ground,  prev_x2, prev_y2, prev_z2);
				D_PUSH_VERTEX(vertices_ground,  prev_x1, prev_y1, prev_z1);


				float pNormal[3];
				calculate_normal(
					&arr_spline_right[(index5-3)],
					&arr_spline_left[(index5-3)],
					&arr_spline_right[index5],
					pNormal
				);

				// std::cout
				// 	<< pNormal[0] << "/"
				// 	<< pNormal[1] << "/"
				// 	<< pNormal[2]
				// 	<< std::endl;

				pNormal[0] *= 10.0f;
				pNormal[1] *= 10.0f;
				pNormal[2] *= 10.0f;

				if (index5 == 3)
				{
					pPreviousNormal[0] = pNormal[0];
					pPreviousNormal[1] = pNormal[1];
					pPreviousNormal[2] = pNormal[2];
				}

				D_PUSH_VERTEX(vertices_walls1,  prev_x1, prev_y1, prev_z1);
				D_PUSH_VERTEX(vertices_walls1,  prev_x1+pPreviousNormal[0], prev_y1+pPreviousNormal[1], prev_z1+pPreviousNormal[2]);
				D_PUSH_VERTEX(vertices_walls1,  x1+pNormal[0], y1+pNormal[1], z1+pNormal[2]);
				D_PUSH_VERTEX(vertices_walls1,  x1, y1, z1);

				D_PUSH_VERTEX(vertices_walls2,  prev_x2, prev_y2, prev_z2);
				D_PUSH_VERTEX(vertices_walls2,  prev_x2+pPreviousNormal[0], prev_y2+pPreviousNormal[1], prev_z2+pPreviousNormal[2]);
				D_PUSH_VERTEX(vertices_walls2,  x2+pNormal[0], y2+pNormal[1], z2+pNormal[2]);
				D_PUSH_VERTEX(vertices_walls2,  x2, y2, z2);

				pPreviousNormal[0] = pNormal[0];
				pPreviousNormal[1] = pNormal[1];
				pPreviousNormal[2] = pNormal[2];

			} // for (int index5 = index; index5 < arr_spline_left.size() && index5 < index + geometry_step * 3; index5 += 3)


			// pPhysicWrapper->createGround(vertices_ground, indices, index);
			// pPhysicWrapper->createGhostWall(vertices_walls1, indices);
			// pPhysicWrapper->createGhostWall(vertices_walls2, indices);

			// if (callback)
			// {
			// 	callback(vertices_ground, indices, true);
			// 	callback(vertices_walls1, indices, false);
			// 	callback(vertices_walls2, indices, false);
			// }

			if (ground)
				ground(vertices_ground, indices);

			if (wall)
			{
				wall(vertices_walls1, indices);
				wall(vertices_walls2, indices);
			}

#undef D_PUSH_VERTEX

    	} // push ground

    } // for (int index = 3; index < arr_spline_left.size(); index += 3 * geometry_step)

	return true;
}
