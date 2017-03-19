#include <iostream>
using namespace std;

#define NUM_SOL 3
#define NUM_COEFF 9

#include <glm/vec3.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

void get_from_user(float *coeff, float *right_hand_side) {
    for(int i = 0, j = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            cin >> coeff[i + j*3];
        }

        cin >> right_hand_side[i];
    }
}

glm::vec3 vec3_from_array(float *s) {
    return glm::vec3(s[0], s[1], s[2]);
}

glm::vec3 solve(glm::mat3 coeff_matrix, glm::vec3 rhs_vector) {
    return glm::inverse(coeff_matrix) * rhs_vector;
}

int main(void) {
    cout << "Upisite koeficijente" << endl;

    float coeff[NUM_COEFF];
    float right_hand_side[NUM_SOL];
    get_from_user(coeff, right_hand_side);

    glm::mat3 coeff_matrix = glm::make_mat3(coeff);
    glm::vec3 right_hand_side_vector = vec3_from_array(right_hand_side);

    if (glm::determinant(coeff_matrix) == 0) {
        cout << "Ne postoji rjesenje!" << endl;
        return -1;
    }

    glm::vec3 solution = solve(coeff_matrix, right_hand_side_vector);
    cout << "Rjesenje je " << glm::to_string(solution) << endl;

    return 0;
}