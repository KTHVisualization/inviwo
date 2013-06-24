#include <inviwo/core/properties/transferfunctionproperty.h>

namespace inviwo {

TransferFunctionProperty::TransferFunctionProperty(std::string identifier, std::string displayName, TransferFunction value, PropertyOwner::InvalidationLevel invalidationLevel, PropertySemantics::Type semantics )
    : TemplateProperty<TransferFunction>(identifier, displayName, value, invalidationLevel, semantics)
{}

void TransferFunctionProperty::serialize(IvwSerializer& s) const {
	Property::serialize(s) ;
	TransferFunction tf = get();
	std::stringstream stream;
	s.serialize("size", (int)tf.getNumberOfDataPoints());

	for (size_t i = 0; i < tf.getNumberOfDataPoints(); i++){
		stream << "pos" << i;
		s.serialize(stream.str(), tf.getPoint(i)->getPos());
		stream.clear();
		stream.str(std::string());

		stream << "rgba" << i;
		s.serialize(stream.str(), tf.getPoint(i)->getRgba());
		stream.clear();
		stream.str(std::string());
	}
}

void TransferFunctionProperty::deserialize(IvwDeserializer& d) {
	Property::deserialize(d);
	int size;
	TransferFunction* tf = &get();
	vec2 pos;
	vec4 rgba;
	std::stringstream stream;

	d.deserialize("size", size);
	for (int i = 0; i < size; i++){
		stream << "pos" << i;
		d.deserialize(stream.str(), pos);
		stream.clear();
		stream.str(std::string());

		stream << "rgba" << i;
		d.deserialize(stream.str(), rgba);
		stream.clear();
		stream.str(std::string());

		tf->addPoint(pos, rgba);
	}
}

void TransferFunctionProperty::customSet(){
	onChangeCallback_.invoke();
	//FIXME: if set() is called before addProperty(), getOwner() will be 0
	getOwner()->invalidate(getInvalidationLevel());
	updatePropertyWidgets();
}

} // namespace
