#include <iostream>
using namespace std;

#include <glm/vec3.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

glm::vec3 vec3_from_array(float *s) {
    return glm::vec3(s[0], s[1], s[2]);
}

glm::vec3 solve(glm::mat3 coeff_matrix, glm::vec3 rhs_vector) {
    return glm::inverse(coeff_matrix) * rhs_vector;
}

glm::mat3 get_triangle_data() {
    float data[9];
    
    for(int i = 0; i < 9; i++) {
        cin >> data[i];
    }

    return glm::transpose(glm::make_mat3(data));
}

glm::vec3 get_point_data() {
    float data[3];

    for(int i = 0; i < 3; i++) {
        cin >> data[i];
    }

    return vec3_from_array(data);
}

int main(void) {
    cout << "Upisite podatke o vrhovima trokuta i tocki" << endl;
    
    glm::mat3 triangle = get_triangle_data();
    glm::vec3 point = get_point_data();
    glm::vec3 solution = solve(triangle, point);

    cout << glm::to_string(solution) << endl;
    return 0;
}