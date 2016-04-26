<?php

namespace Drupal\paragraphs_extra\Widget;

abstract class WidgetBase implements WidgetInterface {
  protected $type;
  protected $field_element;
  protected $form_state;
  protected $context;

  public function __construct($type, &$field_element, &$form_state, $context) {
    $this->type = $type;
    $this->field_element = &$field_element;
    $this->form_state = &$form_state;
    $this->context = $context;
  }

  public function getType() {
    return $this->type;
  }

  public function getElement() {
    if (isset($this->field_element['add_more']['paragraphs_extra']['widget']['widgets'][$this->type])) {
      return $this->field_element['add_more']['paragraphs_extra']['widget']['widgets'][$this->type];
    }
    else {
      return array();
    }
  }

  public function getValues() {
    $element = $this->getElement();
    $values = drupal_array_get_nested_value($this->form_state['values'], $element['#parents']);
    return is_array($values) ? $values : array();
  }

  public function setValues(array $values) {
    $element = $this->getElement();
    form_set_value($element, $values, $this->form_state);
    drupal_array_set_nested_value($this->form_state['input'], $element['#parents'], $values, TRUE);
  }

  public function init(array &$field_element) {
  }

  public function element(array &$element, array $paragraphs) {
  }

  public function render(array $variables, array $element) {
    return drupal_render($element);
  }

  public function validate(array $element, array $values) {
  }

  public function submit(\Drupal\paragraphs_extra\Widget\State $state, array $element, array $values) {
  }

  public function setState(\Drupal\paragraphs_extra\Widget\State $state) {
    return array();
  }
}
