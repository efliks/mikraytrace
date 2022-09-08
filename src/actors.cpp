#include "actors.h"


namespace mrtp {

ActorBase::ActorBase(const StandardBasis& local_basis,
                     std::shared_ptr<TextureMapper> texture_mapper_ptr) :
    local_basis_(local_basis),
    texture_mapper_(texture_mapper_ptr) {

}


MyPixel ActorBase::pick_pixel(const Vector3d& X, const Vector3d& N) const {
    return texture_mapper_->pick_pixel(X, N, local_basis_);
}


}  // namespace mrtp
