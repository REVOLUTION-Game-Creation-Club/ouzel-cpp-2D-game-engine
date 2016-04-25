// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "Node.h"
#include "Engine.h"
#include "SceneManager.h"
#include "Layer.h"
#include "Animator.h"
#include "Camera.h"
#include "Utils.h"
#include "MathUtils.h"
#include "Drawable.h"

namespace ouzel
{
    namespace scene
    {
        Node::Node()
        {

        }

        Node::~Node()
        {

        }

        void Node::visit(const Matrix4& parentTransform, bool parentTransformDirty)
        {
            if (parentTransformDirty)
            {
                updateTransform(parentTransform);
            }

            if (_visible)
            {
                if (_transformDirty)
                {
                    calculateTransform();
                }

                LayerPtr layer = _layer.lock();
                // check if _parent is _layer
                bool isRoot = !_parent.owner_before(_layer) && !_layer.owner_before(_parent);

                if (_children.empty())
                {
                    if (layer && (_globalOrder || isRoot) && checkVisibility())
                    {
                        layer->addToDrawQueue(std::static_pointer_cast<Node>(shared_from_this()));
                    }
                }
                else
                {
                    lock();

                    std::stable_sort(_children.begin(), _children.end(), [](const NodePtr& a, const NodePtr& b) {
                        return a->getZ() > b->getZ();
                    });

                    auto i = _children.begin();
                    NodePtr node;

                    for (; i != _children.end(); ++i)
                    {
                        node = *i;

                        if (!node->_remove)
                        {
                            if (node->getZ() < 0.0f)
                            {
                                node->visit(_transform, _updateChildrenTransform);
                            }
                            else
                            {
                                break;
                            }
                        }
                    }

                    if (layer && (_globalOrder || isRoot) && checkVisibility())
                    {
                        layer->addToDrawQueue(std::static_pointer_cast<Node>(shared_from_this()));
                    }

                    for (; i != _children.end(); ++i)
                    {
                        if (!node->_remove)
                        {
                            node = *i;
                            node->visit(_transform, _updateChildrenTransform);
                        }
                    }

                    unlock();
                }

                _updateChildrenTransform = false;
            }
        }

        void Node::process()
        {
            if (_children.empty())
            {
                draw();
            }
            else
            {
                lock();

                auto i = _children.begin();
                NodePtr node;

                for (; i != _children.end(); ++i)
                {
                    node = *i;

                    if (node->getZ() < 0.0f)
                    {
                        if (!node->isGlobalOrder() && node->isVisible() && node->checkVisibility())
                        {
                            node->draw();
                        }
                    }
                    else
                    {
                        break;
                    }
                }

                draw();

                for (; i != _children.end(); ++i)
                {
                    node = *i;

                    if (!node->isGlobalOrder() && node->isVisible() && node->checkVisibility())
                    {
                        node->draw();
                    }
                }

                unlock();
            }
        }

        void Node::draw()
        {
            if (_transformDirty)
            {
                calculateTransform();
            }

            if (LayerPtr layer = _layer.lock())
            {
                if (layer->getCamera())
                {
                    for (const DrawablePtr& drawable : _drawables)
                    {
                        if (drawable->isVisible())
                        {
                            drawable->draw(layer->getCamera()->getProjection(), _transform);
                        }
                    }
                }
            }
        }

        bool Node::addChild(const NodePtr& node)
        {
            if (NodeContainer::addChild(node))
            {
                node->addToLayer(_layer);
                node->updateTransform(getTransform());

                return true;
            }
            else
            {
                return false;
            }
        }

        bool Node::removeFromParent()
        {
            if (NodeContainerPtr parent = _parent.lock())
            {
                parent->removeChild(std::static_pointer_cast<Node>(shared_from_this()));
                return true;
            }

            return false;
        }

        void Node::setZ(float z)
        {
            _z = z;

            // Currently z does not affect transformation
            //_localTransformDirty = _transformDirty = _inverseTransformDirty = true;
        }

        void Node::setGlobalOrder(bool globalOrder)
        {
            _globalOrder = globalOrder;
        }

        void Node::setPosition(const Vector2& position)
        {
            _position = position;

            _localTransformDirty = _transformDirty = _inverseTransformDirty = true;
        }

        void Node::setRotation(float rotation)
        {
            _rotation = rotation;

            _localTransformDirty = _transformDirty = _inverseTransformDirty = true;
        }

        void Node::setScale(const Vector2& scale)
        {
            _scale = scale;

            _localTransformDirty = _transformDirty = _inverseTransformDirty = true;
        }

        void Node::setOpacity(float opacity)
        {
            _opacity = clamp(opacity, 0.0f, 1.0f);
        }

        void Node::setFlipX(bool flipX)
        {
            _flipX = flipX;

            _localTransformDirty = _transformDirty = _inverseTransformDirty = true;
        }

        void Node::setFlipY(bool flipY)
        {
            _flipY = flipY;

            _localTransformDirty = _transformDirty = _inverseTransformDirty = true;
        }

        void Node::setVisible(bool visible)
        {
            _visible = visible;
        }

        void Node::addToLayer(const LayerWeakPtr& layer)
        {
            _layer = layer;

            if (!layer.expired())
            {
                for (const NodePtr& child : _children)
                {
                    child->addToLayer(layer);
                }
            }
        }

        void Node::removeFromLayer()
        {
            for (const NodePtr& child : _children)
            {
                child->removeFromLayer();
            }

            _layer.reset();
        }

        bool Node::pointOn(const Vector2& position) const
        {
            Vector2 localPosition = convertWorldToLocal(position);

            for (const DrawablePtr& drawable : _drawables)
            {
                if (drawable->pointOn(localPosition))
                {
                    return true;
                }
            }

            return false;
        }

        bool Node::rectangleOverlaps(const Rectangle& rectangle) const
        {
            /*Matrix4 inverse = getInverseTransform();

            Vector3 corners[4] = {
                Vector3(rectangle.left(), rectangle.bottom(), 0.0f),
                Vector3(rectangle.right(), rectangle.bottom(), 0.0f),
                Vector3(rectangle.right(), rectangle.top(), 0.0f),
                Vector3(rectangle.left(), rectangle.top(), 0.0f)
            };

            uint8_t inCorners = 0;

            for (Vector3& corner : corners)
            {
                inverse.transformPoint(corner);

                if (corner.x >= _boundingBox.min.x &&
                    corner.x <= _boundingBox.max.x &&
                    corner.y >= _boundingBox.min.y &&
                    corner.y <= _boundingBox.max.y)
                {
                    return true;
                }

                if (corner.x < _boundingBox.min.x && corner.y < _boundingBox.min.y) inCorners |= 0x01;
                if (corner.x > _boundingBox.max.x && corner.y < _boundingBox.min.y) inCorners |= 0x02;
                if (corner.x > _boundingBox.max.x && corner.y > _boundingBox.max.y) inCorners |= 0x04;
                if (corner.x < _boundingBox.min.x && corner.y > _boundingBox.max.y) inCorners |= 0x08;
            }

            // bounding box is bigger than rectangle
            if (inCorners == 0x0F)
            {
                return true;
            }

            Vector2 boundingBoxCorners[4] = {
                Vector2(_boundingBox.min),
                Vector2(_boundingBox.max.x, _boundingBox.min.y),
                Vector2(_boundingBox.max),
                Vector2(_boundingBox.min.x, _boundingBox.max.y)
            };

            for (uint32_t current = 0; current < 4; ++current)
            {
                uint32_t next = (current == 3) ? 0 : current + 1;

                if (linesIntersect(Vector2(corners[current].x, corners[current].y), Vector2(corners[next].x, corners[next].y), boundingBoxCorners[0], boundingBoxCorners[1]) || // left
                    linesIntersect(Vector2(corners[current].x, corners[current].y), Vector2(corners[next].x, corners[next].y), boundingBoxCorners[1], boundingBoxCorners[2]) || // top
                    linesIntersect(Vector2(corners[current].x, corners[current].y), Vector2(corners[next].x, corners[next].y), boundingBoxCorners[2], boundingBoxCorners[3]) || // right
                    linesIntersect(Vector2(corners[current].x, corners[current].y), Vector2(corners[next].x, corners[next].y), boundingBoxCorners[3], boundingBoxCorners[0])) // bottom
                {
                    return true;
                }
            }*/

            return false;
        }

        const Matrix4& Node::getTransform() const
        {
            if (_transformDirty)
            {
                calculateTransform();
            }

            return _transform;
        }

        const Matrix4& Node::getInverseTransform() const
        {
            if (_transformDirty)
            {
                calculateTransform();
            }

            if (_inverseTransformDirty)
            {
                calculateInverseTransform();
            }

            return _inverseTransform;
        }

        void Node::updateTransform(const Matrix4& parentTransform)
        {
            _parentTransform = parentTransform;
            _transformDirty = _inverseTransformDirty = true;
        }

        Vector2 Node::convertWorldToLocal(const Vector2& position) const
        {
            Vector3 localPosition = position;

            const Matrix4& inverseTransform = getInverseTransform();
            inverseTransform.transformPoint(localPosition);

            return Vector2(localPosition.x, localPosition.y);
        }

        Vector2 Node::convertLocalToWorld(const Vector2& position) const
        {
            Vector3 worldPosition = position;

            const Matrix4& transform = getTransform();
            transform.transformPoint(worldPosition);

            return Vector2(worldPosition.x, worldPosition.y);
        }

        bool Node::checkVisibility() const
        {
            if (const LayerPtr& layer = _layer.lock())
            {
                for (const DrawablePtr& drawable : _drawables)
                {
                    if (drawable->isVisible() &&
                        (drawable->getBoundingBox().isEmpty() ||
                         sharedEngine->getRenderer()->checkVisibility(getTransform(), drawable->getBoundingBox(), layer->getCamera())))
                    {
                        return true;
                    }
                }
            }

            return false;
        }

        void Node::animate(const AnimatorPtr& animator)
        {
            stopAnimation();
            _currentAnimator = animator;

            if (_currentAnimator)
            {
                _currentAnimator->start(std::static_pointer_cast<Node>(shared_from_this()));
            }
        }

        void Node::stopAnimation()
        {
            if (_currentAnimator)
            {
                _currentAnimator->stop();
                removeAnimation();
            }
        }

        void Node::removeAnimation()
        {
            _currentAnimator.reset();
        }

        void Node::calculateLocalTransform() const
        {
            _localTransform = Matrix4::IDENTITY;
            _localTransform.translate(Vector3(_position.x, _position.y, 0.0f));
            _localTransform.rotateZ(TAU - _rotation);

            Vector3 scale = Vector3(_scale.x * (_flipX ? -1.0f : 1.0f),
                                    _scale.y * (_flipY ? -1.0f : 1.0f),
                                    1.0f);

            _localTransform.scale(scale);

            _localTransformDirty = false;
        }

        void Node::calculateTransform() const
        {
            if (_localTransformDirty)
            {
                calculateLocalTransform();
            }

            _transform = _parentTransform * _localTransform;
            _transformDirty = false;

            _updateChildrenTransform = true;
        }

        void Node::calculateInverseTransform() const
        {
            if (_transformDirty)
            {
                calculateTransform();
            }

            _inverseTransform = _transform;
            _inverseTransform.invert();
            _inverseTransformDirty = false;
        }

        void Node::addDrawable(DrawablePtr drawable)
        {
            _drawables.push_back(drawable);
            drawable->setParentNode(std::static_pointer_cast<Node>(shared_from_this()));
        }

        void Node::removeDrawable(uint32_t index)
        {
            if (index >= _drawables.size())
            {
                return;
            }

            _drawables.erase(_drawables.begin() + index);
        }

        void Node::removeDrawable(DrawablePtr drawable)
        {
            for (auto i = _drawables.begin(); i != _drawables.end();)
            {
                if (*i == drawable)
                {
                    i = _drawables.erase(i);
                }
                else
                {
                    ++i;
                }
            }
        }

    } // namespace scene
} // namespace ouzel
