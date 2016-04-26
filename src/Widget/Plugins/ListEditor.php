<?php

namespace Drupal\paragraphs_extra\Widget\Plugins;

use \Drupal\paragraphs_extra\Widget\WidgetBase;

class ListEditor extends WidgetBase {

  public function element(array &$element, array $entities) {
    // The form element for this widget is built by the paragraphs module. There
    // is no need for an additional sub-element.
    $element['#access'] = FALSE;
  }

  public function getElement() {
    return $this->field_element;
  }

  public function render(array $variables, array $element) {
    return theme_paragraphs_field_multiple_value_form($variables);
  }

  public function setValues(array $values) {
    // Nothing to do here.
  }

  public function getValues() {
    return array();
  }

  public function validate(array $element, array $values) {
    // Save a copy of the field state to prevent the default validator from mucking
    // with it.
    $field_state = field_form_get_state($element['#parents'], $element['#field_name'], $element['#language'], $this->form_state);

    // Handles validation.
    foreach ($element as $delta => $item) {
      if (is_numeric($delta)) {
        paragraphs_field_widget_embed_validate($item, $this->form_state, array());
      }
    }

    //Restores the field state.
    field_form_set_state($element['#parents'], $element['#field_name'], $element['#language'], $this->form_state, $field_state);
  }
}
